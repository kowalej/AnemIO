#!/usr/bin/env python3

import asyncio
import datetime
import os
import re
import sqlite3
import sys
import time
import geocoder
from contextlib import closing
from sqlite3 import Connection
from urllib.request import pathname2url

from aiohttp import ClientSession
from RFM69 import FREQ_915MHZ, Radio
from RFM69.registers import REG_RSSITHRESH

from .constants import *

# Sets up the database, returns connection.	
def connect_db(db_name = DEFAULT_DB_NAME):
	# DB setup.
	print('Connecting to database {}...'.format(db_name))
	try:
		dburi = 'file:{}?mode=rw'.format(pathname2url(db_name))
		db_conn = sqlite3.connect(dburi, uri=True)
	except sqlite3.OperationalError:
		print('DB does not exist, creating {}...'.format(db_name))
		db_conn = sqlite3.connect(db_name)
	print('Done.')
	return db_conn

# Gets a Unix epoch millisecond timestamp with optional offset based on radio transmittion time.
def get_ts(timestamp, offset_ms = 0):
	unix_ts = (timestamp - datetime.datetime(1970, 1, 1)).total_seconds()
	return int((unix_ts - (offset_ms / 1000)) * 1000)

class RadioDaemon():
	# Construct the class.
	def __init__(self, db_conn: Connection = None, radio: Radio = None, **kwargs):
		# Database connection object.
		self.db_conn = db_conn

		# Radio object.
		self.radio = radio

		# Online / offline device statuses.
		self.device_statuses = {}

		# Average time it takes to receive data after it was sent by station.
		self.average_radio_delay_ms = kwargs.get('average_radio_delay_ms', DEFAULT_RADIO_DELAY_MS)
		self.receive_sleep_sec = kwargs.get('receive_sleep_sec', DEFAULT_RECEIVE_SLEEP_SEC)

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
	def parse_messages(self, messages):
		data = ''
		timestamps_lookup = []

		# Connect all the data and then mark the timestamp of that message(s) part in a lookup for later.
		data_part_start_index = 0 
		data_part_end_index = -1
		for message in messages:
			data += message['data']
			data_part_start_index = data_part_end_index + 1
			data_part_end_index = data_part_start_index + len(data) - 1
			received_timestamp = message['received_timestamp']
			timestamps_lookup.append({ 'start': data_part_start_index, 'end': data_part_end_index, 'received_timestamp': received_timestamp })

		if len(data) < 1:
			pass
			# TODO: log messages are empty.
		parsed = []
		breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
		if len(breaks) < 1:
			pass
			# TODO: log no control character found in messages (cannot determine message type).
		for i in range(0, len(breaks)):
			radio_command = data[breaks[i][0]:breaks[i][1]].replace('[','').replace(']','')
			end_index = breaks[i+1][0] if i < len(breaks) - 1 else len(data)
			contents = data[breaks[i][1]:end_index]
			received_timestamp = next(x['received_timestamp'] for x in timestamps_lookup if x['start'] >= breaks[i][0] and x['end'] <= (end_index - 1))
			parsed.append({ 'radio_command': RadioCommands(int(radio_command)), 'contents': contents, 'received_timestamp': received_timestamp})
			print(parsed[-1])
		return parsed

	# Apply logic to parsed messages (save to DB, push data, etc).
	def handle_messages(self, parsed_messages):
		with closing(self.db_conn.cursor()) as c:
			for message in parsed_messages:
				command = message['radio_command']
				contents = message['contents']
				timestamp = get_ts(message['received_timestamp'], self.average_radio_delay_ms)
				print(timestamp)

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
						print(self.device_statuses)
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
							pass
							# TODO: log total online does not match reported total
						if int(m.group(2)) != len(offline_devices):
							pass
							# TODO: log total offfline does nt match total reported

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

	# Continuously running radio packet recieve sequence.
	async def _receiver(self, radio):
		compact_collecting = False
		messages_collected = []
		parsed_messages = []

		while True:
			for packet in radio.get_packets():
				print('Packet received:', packet)
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
					parsed_messages = self.parse_messages(messages_collected)
					if len(parsed_messages) > 0:
						self.handle_messages(parsed_messages)
					messages_collected.clear() # Make sure we clear messages now that we have parsed them.

			# TODO: Save data...
			# await call_API("http://httpbin.org/post", packet)
			await asyncio.sleep(self.receive_sleep_sec)

	# Starts up the main data capture loop.
	def start_capturing(self, radio, loop):
		print('Radio settings: NODE = {0}, NETWORK = {1}'.format(NODE, NET))
		print('Listening...')
		loop.run_until_complete(self._receiver(radio))

	# Stops the main loops.
	def stop_capturing(self, loop):
		print('Stopping radio listening...')
		loop.stop()

	# Tries to record the station's location based on this device's IP address.
	def record_location(self):
		latlng = None
		try:
			g = geocoder.ip('me')
			latlng = g.latlng
			print('Found location using IP address! Latitude: {0} / Longitude: {0}'.format(latlng[0], latlng[1]))
		except:
			print('Could not get location using IP address.')
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
		# Try to set OS high priority.
		try:
			os.nice(1)
		except:
			pass

		try:
			# Get our async loop object.
			loop = asyncio.get_event_loop()

			# Get the db connection, if it was not passed in (i.e. testing).
			close_db = False
			if self.db_conn is None:
				self.db_conn = connect_db()
				close_db = True
			
			self._create_db_schema()

			# Record station location estimate, based on our base station's IP address.
			self.record_location()

			# Initialize the radio as a resource, unless a radio was passed in (i.e. testing).
			print('Radio initialization...')
			if self.radio is None:
				with Radio(
						FREQ_915MHZ,
						NODE,
						NET,
						isHighPower = True,
						encryptionKey = ENCRYPT_KEY,
						power = 100,  # Send at 100% power, since we don't care about power saving on this end (we are wired in).
						promiscuous = False,  # We don't care about any messages except our weather station.
						autoAcknowledge = True,  # Station expects Acknowledgements.
						verbose = False
					) as radio:
					print('Done.')
					self.start_capturing(radio, loop)
			else:
				self.start_capturing(self.radio, loop)

		except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
			print('Keyboard interrupt.')
			self.stop_capturing(loop)

		except Exception:
			e = sys.exc_info()
			print('An error occured: {0}'.format(str(e)))
			self.stop_capturing(loop)

		finally:
			print('Shutting down.')
			if close_db and self.db_conn is not None:
				self.db_conn.close()
			if loop is not None:
				loop.close()

if __name__ == '__main__':
    RadioDaemon().run()
