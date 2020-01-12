import asyncio
import datetime
import logging
import os
import re
import sqlite3
import sys
import time
from contextlib import closing
from sqlite3 import Connection
from urllib.request import pathname2url

import geocoder
import requests
from aiohttp import ClientSession
from RFM69 import FREQ_915MHZ, Radio
from RFM69.registers import REG_RSSITHRESH
from sendgrid import SendGridAPIClient
from sendgrid.helpers.mail import Mail

from .constants import *


# Sets up the database, returns connection.	
def connect_db(db_name = DEFAULT_DB_NAME, logger=logging.getLogger()):
	# DB setup.
	logger.info('Connecting to database {}...'.format(db_name))
	try:
		dburi = 'file:{}?mode=rw'.format(pathname2url(db_name))
		db_conn = sqlite3.connect(dburi, uri=True)
	except sqlite3.OperationalError:
		logger.info('DB does not exist, creating {}...'.format(db_name))
		db_conn = sqlite3.connect(db_name)
	logger.info('Done.')
	return db_conn

# Gets a Unix epoch millisecond timestamp with optional offset based on radio transmittion time.
def get_ts(timestamp, offset_ms = 0):
	unix_ts = (timestamp - datetime.datetime(1970, 1, 1)).total_seconds()
	return int((unix_ts - (offset_ms / 1000)) * 1000)

class RadioDaemon():
	# Construct the class.
	def __init__(self, db_conn: Connection = None, radio: Radio = None, logger = logging.getLogger('anemio'), **kwargs):
		# Database connection object.
		self.db_conn = db_conn

		# Radio object.
		self.radio = radio

		# Online / offline device statuses.
		self.device_statuses = {}

		# Average time it takes to receive data after it was sent by station.
		self.average_radio_delay_ms = kwargs.get('average_radio_delay_ms', DEFAULT_RADIO_DELAY_MS)
		self.transceive_sleep_sec = kwargs.get('transceive_sleep_sec', DEFAULT_TRANSRECEIVE_SLEEP_SEC)

		self.logger = logger

	# Sets up the database schema, creating tables if needed.
	def _create_db_schema(self):
		with closing(self.db_conn.cursor()) as c:
			# Create station location table.
			c.execute('''CREATE TABLE IF NOT EXISTS station_location
						(timestamp INTEGER, lat REAL, lon REAL, is_actual INTEGER)''')

			# Create station state table.
			c.execute('''CREATE TABLE IF NOT EXISTS station_state
						(timestamp INTEGER, state INTEGER)''')

			# Create station device state table.
			c.execute(
					'''CREATE TABLE IF NOT EXISTS device_state
						(timestamp INTEGER, 
						online_devices TEXT,
						offline_devices TEXT,
						is_setup INTEGER
					)'''
			)

			# Create ambient light values table.
			c.execute('''CREATE TABLE IF NOT EXISTS ambient_light_values
						(timestamp INTEGER, value REAL)''')

			# Create ambient light state table.
			c.execute('''CREATE TABLE IF NOT EXISTS ambient_light_state
						(timestamp INTEGER, state TEXT)''')

			# Create compass XYZ table.
			c.execute('''CREATE TABLE IF NOT EXISTS compass_xyz
						(timestamp INTEGER, x REAL, y REAL, z REAL)''')

			# Create compass heading table.
			c.execute('''CREATE TABLE IF NOT EXISTS compass_heading
						(timestamp INTEGER, heading INTEGER)''')

			# Create accelerometer XYZ table.
			c.execute('''CREATE TABLE IF NOT EXISTS accelerometer_xyz
						(timestamp INTEGER, x REAL, y REAL, z REAL)''')

			# Create pressure values table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_values
						(timestamp INTEGER, value REAL)''')

			# Create pressure temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_temperature
						(timestamp INTEGER, value REAL)''')

			# Create pressure altitude table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_altitude
						(timestamp INTEGER, value REAL)''')

			# Create rain values table.
			c.execute('''CREATE TABLE IF NOT EXISTS rain_values
						(timestamp INTEGER, value REAL)''')

			# Create rain state table.
			c.execute('''CREATE TABLE IF NOT EXISTS rain_state
						(timestamp INTEGER, state TEXT)''')

			# Create temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS temperature
						(timestamp INTEGER, value REAL)''')

			# Create humidity table.
			c.execute('''CREATE TABLE IF NOT EXISTS humidity
						(timestamp INTEGER, value REAL)''')

			# Create water temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS water_temperature
						(timestamp INTEGER, value REAL)''')

			# Create wind direction table.
			c.execute('''CREATE TABLE IF NOT EXISTS wind_direction
						(timestamp INTEGER, value INTEGER)''')

			# Create wind speed table.
			c.execute('''CREATE TABLE IF NOT EXISTS wind_speed
						(timestamp INTEGER, value REAL, temperature REAL)''')

			# Save changes.
			self.db_conn.commit()

	# Parse one or more messages in packet (multiple messagse occurs with a compact send).
	def _parse_messages(self, messages):
		data = ''
		timestamps_lookup = []

		# Connect all the data and then mark the timestamp of that message(s) part in a lookup for later.
		for message in messages:
			data_part_start_index = len(data)
			data += message['data']
			data_part_end_index = len(data) - 1
			# We only care about the messages where the starting control character '[' exists.
			if '[' in data:
				received_timestamp = message['received_timestamp']
				timestamps_lookup.append({ 'start': data_part_start_index, 'end': data_part_end_index, 'received_timestamp': received_timestamp })
		if len(data) < 1:
			pass
			# TODO: log messages are empty.
		parsed = []
		breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
		if len(breaks) < 1:
			# TODO: log no control characters found in messages (cannot determine message type).
			pass
		for i in range(0, len(breaks)):
			radio_command = data[breaks[i][0]:breaks[i][1]].replace('[','').replace(']','')
			end_index = breaks[i+1][0] if i < len(breaks) - 1 else len(data)
			contents = data[breaks[i][1]:end_index]
			received_timestamp = next(x['received_timestamp'] for x in timestamps_lookup if breaks[i][0] >= x['start'] and breaks[i][0] <= x['end'])
			parsed.append({ 'radio_command': RadioCommands(int(radio_command)), 'contents': contents, 'received_timestamp': received_timestamp})
			self.logger.info(parsed[-1])
		return parsed

	# Apply logic to parsed messages (save to DB, push data, etc).
	def _handle_messages(self, parsed_messages):
		with closing(self.db_conn.cursor()) as c:
			for message in parsed_messages:
				command = message['radio_command']
				contents = message['contents']
				timestamp = get_ts(message['received_timestamp'], self.average_radio_delay_ms)

				if command == RadioCommands.SETUP_START:
					# Station is in booting state.
					c.execute(
						'INSERT INTO station_state(timestamp, state) VALUES (?,?)',
						(timestamp, StationState.BOOTING.value)
					)
				elif command == RadioCommands.REPORT_SETUP_STATE:
					m = re.match('D:([0-9]+)O:([0,1]+)', contents)
					if m is not None:
						self.device_statuses[int(m.group(1))] = True if int(m.group(2)) == 1 else False
						self.logger.info(self.device_statuses)
					else:
						pass
						# TODO: log bad formatted message.
				elif command == RadioCommands.SETUP_FINISH:
					online_devices = []
					offline_devices = []
					for key, value in self.device_statuses.items():
						if(value == True):
							online_devices.append(key)
						else:
							offline_devices.append(key)

					# Record device statuses.
					c.execute(
						'INSERT INTO device_state(timestamp, online_devices, offline_devices, is_setup) VALUES (?,?,?,?)', 
						(timestamp, repr(online_devices), repr(offline_devices), True)
					)

					m = re.match('O:([0-9]+)F:([0-9])', contents)
					if m is not None:
						if int(m.group(1)) != len(online_devices):
							_log
							# TODO: log total online does not match reported total
						if int(m.group(2)) != len(offline_devices):
							pass
							# TODO: log total offline does nt match total reported

						# Station is now online.
						c.execute(
							'INSERT INTO station_state(timestamp, state) VALUES (?,?)', 
							(timestamp, StationState.ONLINE.value)
						)
						self.device_statuses.clear()
					else:
						pass
						# TODO: log bad formatted message.

				# elif command == RadioCommands.REPORT_ONLINE_STATE:
				# elif command == RadioCommands.SAMPLES_START:
				# elif command == RadioCommands.SAMPLE_GROUP_DIVIDER:
				# elif command == RadioCommands.SAMPLE_WRITE:
				# elif command == RadioCommands.SAMPLES_FINISH:
			
			# Save changes.
			self.db_conn.commit()

	def _get_station_state(self):
		with closing(self.db_conn.cursor()) as c:
			c.execute('SELECT state FROM device_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
			return StationState(c.fetchone()[0])

	# Continuously running radio packet recieve sequence.
	async def _transceiver(self, radio):
		compact_collecting = False
		messages_collected = []
		parsed_messages = []

		while True:
			try:
				station_state = self._get_station_state()
				if station_state == StationState.ONLINE:
					for packet in radio.get_packets():
						self.logger.info('Packet received:')
						self.logger.info(packet)
						message = ''.join([chr(letter) for letter in packet.data])

						# See if we have started a long series of messages (compact messaging).
						# Compact messages start with control characters ^^ and end with $$ - which we will remove.
						if(message.startswith(COMPACT_MESSAGES_START)):
							compact_collecting = True
							message = message.lstrip(COMPACT_MESSAGES_START)
						if(message.endswith(COMPACT_MESSAGES_END)):
							compact_collecting = False
							message = message.rstrip(COMPACT_MESSAGES_END)
						# Always add the data to our messages object.
						messages_collected.append({ 'data': message, 'received_timestamp': packet.received })

						if not compact_collecting:
							parsed_messages = self._parse_messages(messages_collected)
							messages_collected.clear() # Make sure we clear messages now that we have parsed them.
							if len(parsed_messages) > 0:
								self._handle_messages(parsed_messages)
				elif station_state == StationState.PENDING_RESTART:
					self.logger.info('Sending restart request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.RESTART.value))
					self.logger.info('Restart request .'.format('successful' if success else 'unsuccessful'))
				elif station_state == StationState.PENDING_SLEEP:
					self.logger.info('Sending sleep request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.SLEEP.value))
					self.logger.info('Sleep request .'.format('successful' if success else 'unsuccessful'))
				elif station_state == StationState.PENDING_WAKE:
					self.logger.info('Sending wake request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.WAKE.value))
					self.logger.info('Wake request .'.format('successful' if success else 'unsuccessful'))
			except Exception as e:
				self.logger.info('An error occured...')
				self.logger.info(sys.exc_info())
			# TODO: Save data...
			# await call_API("http://httpbin.org/post", packet)
			await asyncio.sleep(self.transceive_sleep_sec)

	# Starts up the main data capture loop.
	def start_capturing(self, radio, loop):
		self.logger.info('Radio settings: NODE = {0}, NETWORK = {1}'.format(RADIO_BASE_NODE_ID, NET))
		self.logger.info('Listening...')
		loop.run_until_complete(self._transceiver(radio))

	# Stops the main loops.
	def stop_capturing(self, loop):
		self.logger.info('Stopping radio listening...')
		loop.stop()

	# Tries to record the station's location based on this device's IP address.
	def record_location(self):
		latlng = None
		try:
			with requests.Session() as session:
				self.logger.info('Attempting to get (estimated) location automatically using base station IP address.')
				g = geocoder.ip('me', session=session)
				latlng = g.latlng
				self.logger.info('Found location using IP address! Latitude: {0} / Longitude: {0}'.format(latlng[0], latlng[1]))
		except:
			self.logger.info('Could not get location using IP address.')
			pass
		if latlng is not None:
			with closing(self.db_conn.cursor()) as c:
				c.execute('INSERT INTO station_location(timestamp, lat, lon, is_actual) VALUES(?,?,?,?)', 
					(
						get_ts(datetime.datetime.utcnow()),
						latlng[0],
						latlng[1],
						0
					)
				)

	# Main sequence.
	def run(self):
		try:
			# Get our async loop object.
			loop = asyncio.get_event_loop()

			# Get the db connection, if it was not passed in (i.e. testing).
			close_db = False
			if self.db_conn is None:
				self.db_conn = connect_db(logger=self.logger)
				close_db = True
			
			self._create_db_schema()

			# Record station location estimate, based on our base station's IP address.
			self.record_location()

			# Initialize the radio as a resource, unless a radio was passed in (i.e. testing).
			self.logger.info('Radio initialization...')
			if self.radio is None:
				with Radio(
						FREQ_915MHZ,
						RADIO_BASE_NODE_ID,
						NET,
						isHighPower = True,
						encryptionKey = ENCRYPT_KEY,
						power = 100,  # Send at 100% power, since we don't care about power saving on this end (we are wired in).
						promiscuous = False,  # We don't care about any messages except our weather station.
						autoAcknowledge = True,  # Station expects Acknowledgements.
						verbose = False
					) as radio:
					self.logger.info('Done.')
					self.start_capturing(radio, loop)
			else:
				self.start_capturing(self.radio, loop)

		except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
			self.logger.info('Keyboard interrupt.')
			self.stop_capturing(loop)

		except Exception:
			e = sys.exc_info()
			self.logger.info('An error occured: {0}'.format(str(e)))
			self.stop_capturing(loop)

		finally:
			self.logger.info('Shutting down.')
			if close_db and self.db_conn is not None:
				self.db_conn.close()
			if loop is not None:
				loop.close()
