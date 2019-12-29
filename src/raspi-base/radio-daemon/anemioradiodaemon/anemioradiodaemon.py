#!/usr/bin/env python3

import asyncio
import datetime
import os
import re
import sqlite3
import sys
import time
from contextlib import closing
from urllib.request import pathname2url

from aiohttp import ClientSession
from RFM69 import FREQ_915MHZ, Radio
from RFM69.registers import REG_RSSITHRESH

from .constants import *

DB_NAME = 'anemio.db'
db_conn = None

# Station state stuff.
device_statuses = {}

def create_db():
	with closing(db.cursor()) as c:
		c = conn.cursor()

		# Create station location table.
		c.execute('''CREATE TABLE station_location
					(timestamp INTEGER, lat REAL, lon REAL)''')

		# Create station state table.
		c.execute('''CREATE TABLE station_state
					(timestamp INTEGER, state INTEGER)''')

		# Create station device state table.
		c.execute(
				'''CREATE TABLE device_state
					(timestamp INTEGER, 
					online_devices TEXT,
					offline_devices TEXT
				)'''
		)

		# Create ambient light values table.
		c.execute('''CREATE TABLE ambient_light_values
					(timestamp INTEGER, value REAL)''')

		# Create ambient light state table.
		c.execute('''CREATE TABLE ambient_light_state
					(timestamp INTEGER, state TEXT)''')

		# Create compass XYZ table.
		c.execute('''CREATE TABLE compass_xyz
					(timestamp INTEGER, x REAL, y REAL, z REAL)''')

		# Create compass heading table.
		c.execute('''CREATE TABLE compass_heading
					(timestamp INTEGER, heading INTEGER)''')

		# Create accelerometer XYZ table.
		c.execute('''CREATE TABLE accelerometer_xyz
					(timestamp INTEGER, x REAL, y REAL, z REAL)''')

		# Create pressure values table.
		c.execute('''CREATE TABLE pressure_values
					(timestamp INTEGER, value REAL)''')

		# Create pressure temperature table.
		c.execute('''CREATE TABLE pressure_temperature
					(timestamp INTEGER, value REAL)''')

		# Create pressure altitude table.
		c.execute('''CREATE TABLE pressure_altitude
					(timestamp INTEGER, value REAL)''')

		# Create rain values table.
		c.execute('''CREATE TABLE rain_values
					(timestamp INTEGER, value REAL)''')

		# Create rain state table.
		c.execute('''CREATE TABLE rain_state
					(timestamp INTEGER, state TEXT)''')

		# Create temperature table.
		c.execute('''CREATE TABLE temperature
					(timestamp INTEGER, value REAL)''')

		# Create humidity table.
		c.execute('''CREATE TABLE humidity
					(timestamp INTEGER, value REAL)''')

		# Create water temperature table.
		c.execute('''CREATE TABLE water_temperature
					(timestamp INTEGER, value REAL)''')

		# Create wind direction table.
		c.execute('''CREATE TABLE wind_direction
					(timestamp INTEGER, value INTEGER)''')

		# Create wind speed table.
		c.execute('''CREATE TABLE wind_speed
					(timestamp INTEGER, value REAL, temperature REAL)''')

		# Save changes.
		db_conn.commit()

# Parse one or more messages in packet (multiple occurs with compact send).
def parse_messages(messages):
	data = ''
	timestamps_lookup = []

	# Connect all the data and then mark the timestamp of that message(s) part in a lookup for later.
	data_part_start_index = 0 
	data_part_end_index = -1
	for message in messages:
		data += message.data
		data_part_start_index = data_part_end_index + 1
		data_part_end_index = data_part_start_index + len(data) - 1
		timestamps_lookup.push({ 'start': data_part_start_index, 'end': data_part_end_index, 'received_timestamp': receieved_timestamp })

	if len(data < 1):
		pass
		# TODO: log messages are empty.
	parsed = []
	breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
	if len(breaks < 1):
		pass
		# TODO: log no control character found in messages (cannot determine message type).
	for i in range(0, len(breaks)):
		radio_command = data[breaks[i][0]:breaks[i][1]].replace('[','').replace(']','')
		end_index = breaks[i+1][0] if i < len(breaks) - 1 else len(data)
		contents = data[breaks[i][1]:end_index]
		received_timestamp = next(x for x in timestamps_lookup if x.start >= breaks[i][0] and x.end <= (end_index - 1))
		parsed.append({ 'radio_command': RadioCommands(radio_command), 'contents': contents, 'received_timestamp': received_timestamp})
		print(parsed[-1])
	return parsed

def handle_messages(parsed_messages):
	with closing(db.cursor()) as c:
		c = conn.cursor()

		for message in parsed_messages:
			if message.radio_command == RadioCommands.SETUP_START:
				# Station is in booting state.
				c.execute(
					'INSERT INTO station_state(timestamp, state) VALUES (?,?)',
					(message.receieved_timestamp, StationStatus.BOOTING)
				)
			elif message.radio_command == RadioCommands.REPORT_SETUP_STATE:
				m = re.match('D:([0-9]+)O:([1,2]+)')
				if m is not None:
					device_statuses[m.groups[0]] = m.groups[1]
				else:
					pass
					# TODO: log bad formatted message.
			elif message.radio_command == RadioCommands.SETUP_FINISH:
				online_devices = []
				offline_devices = []
				for k in device_statuses:
					if(device_statuses[k] == True):
						online.append(k)
					else:
						offline.append(k)

				# Record device statuses.
				c.execute(
					'INSERT INTO device_state(timestamp, online_devices, offline_devices) VALUES (?,?,?)', 
					(message.receieved_timestamp, online_devices, offline_devices)
				)

				m = re.match('N:([0-9]+)')
				if m is not None:
					# Station is now online.
					c.execute(
						'INSERT INTO station_state(timestamp, state) VALUES (?,?)', 
						(message.receieved_timestamp, StationStatus.ONLINE)
					)
					device_statuses.clear()
				else:
					pass
					# TODO: log bad formatted message.

			# elif message.radio_command == RadioCommands.REPORT_ONLINE_STATE:
			# elif message.radio_command == RadioCommands.SAMPLES_START:
			# elif message.radio_command == RadioCommands.SAMPLE_GROUP_DIVIDER:
			# elif message.radio_command == RadioCommands.SAMPLE_WRITE:
			# elif message.radio_command == RadioCommands.SAMPLES_FINISH:
		
		# Save changes.
		db_conn.commit()

async def receiver(radio):
	compact_collecting = False
	messages_collected = []
	parsed_messages = []

	while True:
		for packet in radio.get_packets():
			message = ''.join([chr(letter) for letter in packet.data])
			print('Packet received:', packet)
			#print(message)

			# See if we have started a long series of messages (compact messaging).
			# Compact messages start with control characters ^^ and end with $$ - which we will remove.
			if(message.startswith(COMPACT_MESSAGES_START)):
				compact_collecting = True
				message = message.lstrip(COMPACT_MESSAGES_START)
			if(message.endswith(COMPACT_MESSAGES_END)):
				compact_collecting = False
				message = message.rstrip(COMPACT_MESSAGES_END)

			# Always add the data to our messages object.
			messages_collected.append({ 'data': data, 'received_timestamp': packet.received })

			if not compact_collecting:
				parsed_messages = parse_messages(messages_collected)
				if len(parsed_messages) > 0:
					handle_messages(parsed_messages)
				messages_collected.clear() # Make sure we clear messages now that we have parsed them.

		# TODO: Save data...
		# await call_API("http://httpbin.org/post", packet)
		await asyncio.sleep(TOSLEEP)

def start_listening(radio, loop):
	print('Radio settings: NODE = {0}, NETWORK = {1}'.format(NODE, NET))
	print('Listening...')
	loop.run_until_complete(receiver(radio))
	
def setup_db(db_name):
	# DB setup.
	print('Connecting to database {}...'.format(db_name))
	try:
		dburi = 'file:{}?mode=rw'.format(pathname2url(db_name))
		db_conn = sqlite3.connect(dburi, uri=True)
	except sqlite3.OperationalError:
		print('DB does not exist, creating {}...'.format(db_name))
		db_conn = sqlite3.connect(db_name)
		create_db()
	print('Done.')
	return db_conn

# Main sequence.
def main():
	# Try to set OS high priority.
	try:
		os.nice(1)
	except:
		pass

	try:
		# Get our database.
		db_conn = setup_db(DB_NAME)

		# Get our async loop object.
		loop = asyncio.get_event_loop()

		# Initialize the radio as a resource.
		print('Radio initialization...')
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
			start_listening(radio, loop)

	except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
		print('Keyboard interrupt.')

	except Exception:
		e = sys.exc_info()[0]
		print('An error occured: {0}'.format(str(e)))

	finally:
		print('Shutting down.')
		db_conn.close()
		loop.close()

if __name__ == '__main__':
    unittest.main()
