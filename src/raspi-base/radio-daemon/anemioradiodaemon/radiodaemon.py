import asyncio
import datetime
import logging
import os
import re
import sqlite3
import sys
import time
from datetime import timezone, datetime
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
	if os.path.isfile(db_name):
		logger.info('DB does not exist, creating {}...'.format(db_name))
	db_conn = sqlite3.connect(db_name, uri=True)
	logger.info('Done.')
	return db_conn

# Gets a Unix epoch millisecond timestamp with optional offset based on radio transmittion time.
def get_ts(timestamp, offset_ms = 0):
	unix_ts = (timestamp - datetime(1970, 1, 1, tzinfo=timezone.utc)).total_seconds()
	return int((unix_ts - (offset_ms / 1000)) * 1000)

class RadioDaemon():
	# Construct the class.
	def __init__(self, db_conn: Connection = None, radio: Radio = None, logger = logging.getLogger('anemio'), **kwargs):
		# Database connection object.
		self.db_conn = db_conn

		# Event loop.
		self.loop = None

		# Radio object.
		self.radio = radio

		# Last radio message received timestamp.
		self.radio_last_receive:datetime = None

		# Online / offline device statuses.
		self.device_statuses = {}

		# Timestamp for tracking sample set.
		self.samples_start_timestamp = 0  # Real time.
		self.samples_start_station_timestamp = 0  # Millis() on station.
		# Current sample group values.
		self.current_sample_group = None
		self.current_sample_group_db_table = ''
		self.current_sample_group_format = ''
		self.current_sample_group_db_format = ''
		self.current_sample_group_db_value_specifier = ''
		self.current_sample_group_expected_count = 0
		self.current_sample_group_sample_count = 0
		self.current_sample_group_base_time_offset = 0

		# Average time it takes to receive data after it was sent by station.
		self.average_radio_delay_ms = kwargs.get('average_radio_delay_ms', DEFAULT_RADIO_DELAY_MS)
		self.transceive_sleep_sec = kwargs.get('transceive_sleep_sec', DEFAULT_TRANSRECEIVE_SLEEP_SEC)

		self.logger = logger

		# Get the db connection, if it was not passed in (i.e. testing).
		self.close_db = False
		if self.db_conn is None:
			self.db_conn = connect_db(logger=self.logger)
			self.close_db = True
		
		self._create_db_schema()

		self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.UNKNOWN)

	def __del__(self):
		if self.close_db and self.db_conn is not None:
			self.db_conn.close()

	# Sets up the database schema, creating tables if needed.
	def _create_db_schema(self):
		with closing(self.db_conn.cursor()) as c:
			# Create station location table.
			c.execute('''CREATE TABLE IF NOT EXISTS station_location (timestamp INTEGER NOT NULL, lat REAL, lon REAL, is_actual INTEGER)''')
			c.execute('''CREATE INDEX IF NOT EXISTS station_location_index ON station_location (timestamp)''')

			# Create station state table.
			c.execute('''CREATE TABLE IF NOT EXISTS station_state (timestamp INTEGER NOT NULL, state INTEGER)''')
			c.execute('''CREATE INDEX IF NOT EXISTS station_state_index ON station_state (timestamp)''')

			# Create station device state table.
			c.execute('''CREATE TABLE IF NOT EXISTS device_state
					(
						timestamp INTEGER NOT NULL, 
						online_devices TEXT,
						offline_devices TEXT,
						is_setup INTEGER
					)'''
			)
			c.execute('''CREATE INDEX IF NOT EXISTS device_state_index ON device_state (timestamp)''')

			# Create accelerometer XYZ table.
			c.execute('''CREATE TABLE IF NOT EXISTS accelerometer_xyz (timestamp INTEGER NOT NULL, x REAL, y REAL, z REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS accelerometer_xyz_index ON accelerometer_xyz(timestamp)''')

			# Create battery level table.
			c.execute('''CREATE TABLE IF NOT EXISTS battery_level (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS battery_level_index ON battery_level (timestamp)''')

			# Create ambient light values table.
			c.execute('''CREATE TABLE IF NOT EXISTS ambient_light_values (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS ambient_light_values_index ON ambient_light_values (timestamp)''')

			# Create ambient light state table.
			c.execute('''CREATE TABLE IF NOT EXISTS ambient_light_state (timestamp INTEGER NOT NULL, value TEXT)''')
			c.execute('''CREATE INDEX IF NOT EXISTS ambient_light_state_index ON ambient_light_state (timestamp)''')

			# Create compass XYZ table.
			c.execute('''CREATE TABLE IF NOT EXISTS compass_xyz (timestamp INTEGER NOT NULL, x REAL, y REAL, z REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS compass_xyz_index ON compass_xyz (timestamp)''')

			# Create compass heading table.
			c.execute('''CREATE TABLE IF NOT EXISTS compass_heading (timestamp INTEGER NOT NULL, value INTEGER)''')
			c.execute('''CREATE INDEX IF NOT EXISTS compass_heading_index ON compass_heading (timestamp)''')

			# Create pressure values table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_pressure (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS pressure_pressure_index ON pressure_pressure(timestamp)''')

			# Create pressure temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_temperature (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS pressure_temperature_index ON pressure_temperature (timestamp)''')

			# Create pressure altitude table.
			c.execute('''CREATE TABLE IF NOT EXISTS pressure_altitude (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS pressure_altitude_index ON pressure_altitude (timestamp)''')

			# Create rain values table (for onboard rain sensor which can detect raindrops).
			c.execute('''CREATE TABLE IF NOT EXISTS rain_values (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS rain_values_index ON rain_values (timestamp)''')

			# Create rain state table (for state estimated onboard by rain sensor values).
			c.execute('''CREATE TABLE IF NOT EXISTS rain_state (timestamp INTEGER NOT NULL, value TEXT)''')
			c.execute('''CREATE INDEX IF NOT EXISTS rain_state_index ON rain_state (timestamp)''')

			# Create temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS temperature (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS temperature_index ON temperature (timestamp)''')

			# Create humidity table.
			c.execute('''CREATE TABLE IF NOT EXISTS humidity (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS humidity_index ON humidity (timestamp)''')

			# Create water temperature table.
			c.execute('''CREATE TABLE IF NOT EXISTS water_temperature (timestamp INTEGER NOT NULL, value REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS water_temperature_index ON water_temperature (timestamp)''')

			# Create wind direction table.
			c.execute('''CREATE TABLE IF NOT EXISTS wind_direction (timestamp INTEGER NOT NULL, value INTEGER)''')
			c.execute('''CREATE INDEX IF NOT EXISTS wind_direction_index ON wind_direction (timestamp)''')

			# Create wind speed table.
			c.execute('''CREATE TABLE IF NOT EXISTS wind_speed (timestamp INTEGER NOT NULL, value REAL, tc REAL)''')
			c.execute('''CREATE INDEX IF NOT EXISTS wind_speed_index ON wind_speed (timestamp)''')

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
			logging.error('Trying to parse empty message.')
			return None
		parsed = []
		breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
		if len(breaks) < 1:
			logging.error('No control characters found in messages (cannot determine message type). Msg: %s', data)
			return None
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

		# Local functions.
		# ----------------
		def log_bad_command_format(command, contents):
			self.logger.error('Bad command message format when executing %s command. Contents: %s.', str(command), contents)

		def refine_average_radio_delay(roundtrip_average):
			# Ignore large outliers.
			if not (roundtrip_average > (self.average_radio_delay_ms * 3)):
				self.average_radio_delay_ms = int((self.average_radio_delay_ms + roundtrip_average) / 2)

		def end_sample_group():
			if(self.current_sample_group is None):
				return
			if(self.current_sample_group_sample_count != self.current_sample_group_expected_count):
				self.logger.error(
					'Expected samples in group: %d did not match actual samples in group: %d.',
					self.current_sample_group_expected_count,
					self.current_sample_group_sample_count
				)
			self.logger.info('Done processing sample group: %s.', str(self.current_sample_group))
		# End local functions.

		# Command handling routing.
		with closing(self.db_conn.cursor()) as c:
			for message in parsed_messages:
				command = message['radio_command']
				contents = message['contents']
				timestamp = get_ts(message['received_timestamp'], self.average_radio_delay_ms)

				# Station is starting or returning from wake (devices will be initialized).
				if command == RadioCommands.SETUP_START:
					m = re.match('I:([0,1])', contents)
					if m is not None:
						boot_type = StationState.SETUP_BOOT if int(m.group(1)) == 1 else StationState.SETUP_WAKE
						# Station is in a booting state.
						c.execute(
							'INSERT INTO station_state(timestamp, state) VALUES (?,?)',
							(timestamp, boot_type.value)
						)
						self.device_statuses.clear()
					else:
						log_bad_command_format(command, contents)

				# Device has initialized (successfully or not).
				elif command == RadioCommands.REPORT_SETUP_STATE:
					m = re.match('D:([0-9]+)O:([0,1])', contents)
					if m is not None:
						self.device_statuses[int(m.group(1))] = True if int(m.group(2)) == 1 else False
						self.logger.info('Device statuses:')
						self.logger.info(self.device_statuses)
					else:
						log_bad_command_format(command, contents)
				
				# Setup has finished, all devices should be reported as setup successful or unsuccessful.
				elif command == RadioCommands.SETUP_FINISH:
					# Parse device statuses to separate online from offline.
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
						# Sanity checks.
						actual_num_online = int(m.group(1))
						captured_num_online = len(online_devices)
						if actual_num_online != captured_num_online:
							self.logger.error('Total online devices (%d) does not match total from previous messages (%d).', actual_num_online, captured_num_online)
						actual_num_offline = int(m.group(2))
						captured_num_offline = len(offline_devices)
						if actual_num_offline != captured_num_offline:
							self.logger.error('Total offline devices (%d) does not match total from previous messages (%d).', actual_num_offline, captured_num_offline)

						# Station is now online.
						c.execute(
							'INSERT INTO station_state(timestamp, state) VALUES (?,?)', 
							(timestamp, StationState.ONLINE.value)
						)
					else:
						log_bad_command_format(command, contents)
				
				# Device randomly comes online or offline.
				elif command == RadioCommands.REPORT_ONLINE_STATE:
					m = re.match('D:([0-9]+)O:([0,1])', contents)
					if m is not None:
						self.device_statuses[int(m.group(1))] = True if int(m.group(2)) == 1 else False
						self.logger.info('Device statuses:')
						self.logger.info(self.device_statuses)
						
						# Parse device statuses to separate online from offline.
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
					else:
						log_bad_command_format(command, contents)
				
				# A series of samples has started.
				elif command == RadioCommands.SAMPLES_START:
					self.current_sample_group = None
					self.samples_start_timestamp = timestamp
					m = re.match('T:([0-9]+)', contents)
					if m is not None:
						self.samples_start_station_timestamp = int(m.group(1))
					else:
						log_bad_command_format(command, contents)

				# New group of samples (series of readings from a device or some metric).
				elif command == RadioCommands.SAMPLE_GROUP_DIVIDER:
					end_sample_group()
					m = re.match('^S:([0-9]+)F:([A-Z,]+)N:([0-9]+)B:([0-9]+)R:((?:-1|[0-9]+)(?:\.[0-9]+)?)$', contents)
					if m is not None:
						self.current_sample_group = SensorCategory(int(m.group(1)))
						self.current_sample_group_db_table = self.current_sample_group.name.lower()
						self.current_sample_group_format = m.group(2)
						self.current_sample_group_db_format = self.current_sample_group_format.lower().replace('t,', 'timestamp,').replace('v', 'value')
						self.current_sample_group_db_value_specifier = ','.join(['?' for x in self.current_sample_group_format.split(',')])
						self.current_sample_group_expected_count = int(m.group(3))
						self.current_sample_group_sample_count = 0
						self.current_sample_group_base_time_offset = int(m.group(4))
						refine_average_radio_delay(float(m.group(5)))
						self.logger.info('Processing sample group: %s.', self.current_sample_group)
					else:
						log_bad_command_format(command, contents)

				# Single sample from device.
				elif command == RadioCommands.SAMPLE_WRITE:
					samples = contents.split('|')
					self.current_sample_group_sample_count += len(samples)
					for sample in samples:
						values = sample.split(',')
						try:
							if(len(values) == len(self.current_sample_group_format.split(','))):
								real_timestamp = self.samples_start_timestamp - self.current_sample_group_base_time_offset + int(values[0])
								c.execute(
									'INSERT INTO {}({}) VALUES ({})'.format(
										self.current_sample_group_db_table,
										self.current_sample_group_db_format,
										self.current_sample_group_db_value_specifier
									),
									(real_timestamp,) + tuple(values[1:])
								)
								self.logger.info('Pushed sample %s to db table: %s.', sample, self.current_sample_group_db_table)
							else:
								self.logger.error('Bad sample: %s, does not match length for expected sample format: %s.', sample, self.current_sample_group_format)
						except Exception:
							self.logger.error('Bad sample: %s, exception thrown during parsing: %s.', sample, contents, exc_info=1)

				# End of a series of samples.
				elif command == RadioCommands.SAMPLES_FINISH:
					end_sample_group()

				# Station is done calibrating, so set it back online.
				elif command == RadioCommands.CALIBRATION_FINISHED:
					# Station is back online.
					c.execute(
						'INSERT INTO station_state(timestamp, state) VALUES (?,?)',
						(timestamp, StationState.ONLINE.value)
					)

			# Save changes.
			self.db_conn.commit()

	def _get_station_state(self):
		with closing(self.db_conn.cursor()) as c:
			c.execute('SELECT state FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
			r = c.fetchall()
			if len(r) < 1:
				return StationState.UNREACHABLE
			return StationState(r[0][0])
		
	def _set_station_state(self, timestamp, state):
		with closing(self.db_conn.cursor()) as c:
			c.execute(
				'INSERT INTO station_state(timestamp, state) VALUES (?,?)',
				(timestamp, state.value)
			)
			self.db_conn.commit()

	# Continuously running radio packet recieve sequence.
	async def _transceiver(self, radio):
		compact_collecting = False
		messages_collected = []
		parsed_messages = []

		while True:
			try:
				station_state = self._get_station_state()

				# If we are in online, setting up, or unreachable state, we will check for packets.
				if station_state in [
					StationState.UNKNOWN,
					StationState.ONLINE,
					StationState.SETUP_BOOT,
					StationState.SETUP_WAKE,
					StationState.RESTARTING,
					StationState.SLEEPING,
					StationState.UNREACHABLE,
					StationState.CALIBRATING
				]:
					if station_state not in [StationState.UNREACHABLE, StationState.SLEEPING, StationState.UNKNOWN]:
						# Check for no signal timeout.
						if self.radio_last_receive is not None:
							msg_delta = (datetime.now(timezone.utc) - self.radio_last_receive)
							if msg_delta.total_seconds() > MAX_NO_RECEIVE_SECONDS:
								self.logger.critical(
									'Station has become unreachable. Last message received at: %s. Current datetime: %s.',
									str(self.radio_last_receive),
									datetime.now(timezone.utc).strftime()
								)
								self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.UNREACHABLE)
					for packet in radio.get_packets():
						packet.received = packet.received.replace(tzinfo=timezone.utc)
						self.radio_last_receive = packet.received

						# Back online baby (we received a packet after being unreachable, or sleeping, although unusual).
						if station_state == StationState.UNREACHABLE or station_state == StationState.SLEEPING:
							self.logger.critical(
								'Station has come back online after being unreachable. Current datetime: %s.',
								datetime.now(timezone.utc).strftime()
							)
							self._set_station_state(get_ts(packet.received, self.average_radio_delay_ms), StationState.ONLINE)

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
							if parsed_messages and len(parsed_messages) > 0:
								self._handle_messages(parsed_messages)

				# Requested restart, so we try to send the restart command to the station.
				elif station_state == StationState.RESTART_REQUESTED:
					self.logger.info('Sending restart request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.RESTART.value))
					self.logger.info('Restart request %s.', 'successful' if success else 'unsuccessful')
					if success:
						self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.RESTARTING)

				# Requested sleep, so we try to send the sleep command to the station.
				elif station_state == StationState.SLEEP_REQUESTED:
					self.logger.info('Sending sleep request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.SLEEP.value))
					self.logger.info('Sleep request %s.', 'successful' if success else 'unsuccessful')
					if success:					
						self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.SLEEPING)

				# Requested wake, so we try to send the wake command to the station.
				elif station_state == StationState.WAKE_REQUESTED:
					self.logger.info('Sending wake request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.WAKE.value))
					self.logger.info('Wake request %s.', 'successful, setup should now begin' if success else 'unsuccessful')
					if success:
						self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.ONLINE)

				# Requested calibration, so we try to send the calibrate command to the station.
				elif station_state == StationState.CALIBRATE_REQUESTED:
					self.logger.info('Sending calibrate request to station.')
					success = radio.send(RADIO_STATION_NODE_ID, str(RadioCommands.CALIBRATE.value))
					self.logger.info('Calibrate request %s.', 'successful, setup should now begin' if success else 'unsuccessful')
					if success:
						self._set_station_state(get_ts(datetime.now(timezone.utc)), StationState.CALIBRATING)

			except asyncio.CancelledError:
				self.logger.info('Radio transceiving cancelled.')
				raise

			except Exception as e:
				self.logger.info('Radio transceiving error', exc_info=1)

			# Loop sleep.
			await asyncio.sleep(self.transceive_sleep_sec)

	# Starts up the main data capture loop.
	def start_daemon(self):
		self.logger.info('Radio settings: NODE = {0}, NETWORK = {1}'.format(RADIO_BASE_NODE_ID, NET))
		self.logger.info('Listening...')
		if self.loop is not None:
			self.loop.run_until_complete(self._transceiver(self.radio))

	# Stops the main loops.
	def stop_daemon(self):
		self.logger.info('Stopping radio listening...')
		if self.loop is not None:
			self.loop.stop()

	# Shuts down the daemon.
	def shutdown(self):
		self.logger.info('Shutting down.')
		if self.loop is not None:
			self.loop.close()
		exit(0)

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
						get_ts(datetime.now(timezone.utc)),
						latlng[0],
						latlng[1],
						0
					)
				)

	# Main sequence.
	def run(self):
		try:
			# Get our async loop object.
			self.loop = asyncio.get_event_loop()

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
					self.radio = radio
					self.logger.info('Done.')
					self.start_daemon()
			else:
				self.start_daemon()

		except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
			self.logger.info('Keyboard interrupt.')
			self.stop_daemon()

		except Exception:
			self.logger.info('An error occured: {0}.', exc_info=1)
			self.stop_daemon()

		finally:
			self.shutdown()
