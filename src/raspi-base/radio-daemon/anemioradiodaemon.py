#!/usr/bin/env python3

from RFM69 import Radio, FREQ_915MHZ
from RFM69.registers import REG_RSSITHRESH
import datetime
import asyncio
from aiohttp import ClientSession
import time
import sqlite
import sys
import re
from urllib.request import pathname2url
from constants import *

# Set high priority.
import os
os.nice(1)

DB_NAME = 'anemio.db'
dbConn = None

def create_db():
	c = conn.cursor()

	# Create station table.
	c.execute('''CREATE TABLE station_state
				(location text, status int, devices_online text, devices_offline text)''')

	# Create ambient light values table.
	c.execute('''CREATE TABLE ambient_light_values
				(timestamp date, value real)''')

	# Create ambient light state table.
	c.execute('''CREATE TABLE ambient_light_state
				(timestamp date, state text)''')

	# Create compass XYZ table.
	c.execute('''CREATE TABLE compass_xyz
				(timestamp date, x real, y real, z real)''')

	# Create compass heading table.
	c.execute('''CREATE TABLE compass_heading
				(timestamp date, heading int)''')

	# Create accelerometer XYZ table.
	c.execute('''CREATE TABLE accelerometer_xyz
				(timestamp date, x real, y real, z real)''')

	# Create accelerometer XYZ table.
	c.execute('''CREATE TABLE accelerometer_xyz
				(timestamp date, x real, y real, z real)''')

    PRESSURE = 3,
    RAIN = 4,
    TEMPERATURE = 5,
    HUMIDITY = 6,
    WATER_TEMPERATURE = 7,
    WIND_DIRECTION = 8,
    WIND_SPEED = 9,

	# Save changes.
	dbConn.commit()

# Parse one or more messages in packet (multiple occurs with compact send).
def parse_messages(messages):
	data = ''.join[message.data for message in messages]
	if len(data < 1):
		# TODO: log message is empty.
	parsed = []
	breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
	if len(breaks < 1):
		# TODO: log no control character found in messages (cannot determine message type).
	for i in range(0, len(breaks)):
		radio_command = data[breaks[i][0]:breaks[i][1]]
		end_index = breaks[i+1][0] if i < len(breaks) - 1 else len(data)
		contents = data[breaks[i][1]:end_index]
		parsed.append({ 'radio_command': (RadioCommands)radio_command, 'contents': contents })
		print(parsed[-1])
	return parsed

def handle_messages(parsed_messages):
	for message in parsed_messages:
		if message.radio_command == RadioCommands.SETUP_START:


		SETUP_START = 1,
		SETUP_FINISH = 2,
		REPORT_ONLINE_STATE = 3,
		REPORT_SETUP_STATE = 4,
		SAMPLES_START = 5,
		SAMPLE_GROUP_DIVIDER = 6,
		SAMPLE_WRITE = 7,
		SAMPLES_FINISH = 8

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
			# Compact messages start with ^^ and end with $$ - which we will remove.
			if(message.startswith(COMPACT_MESSAGES_START)):
				compact_collecting = True
				message = message.lstrip(COMPACT_MESSAGES_START)
			if(message.endswith(COMPACT_MESSAGES_END)):
				compact_collecting = False
				message = message.rstrip(COMPACT_MESSAGES_END)

			# Always add the data to our messages object.
			messages_collected.append({ 'data': data, 'timestamp': packet.received })

			if not compact_collecting:
				parsed_messages = parse_messages(messages_collected)
				if len(parsed_messages) > 0:
					handle_messages(parsed_messages)
				messages_collected.clear() # Make sure we clear messages now that we have parsed them.

		# TODO: Save data...
	 	# await call_API("http://httpbin.org/post", packet)
		await asyncio.sleep(TOSLEEP)

# Main sequence.
try:
	# DB setup.
	print('Connecting to database {}...'.format(DB_NAME))
	try:
		dburi = 'file:{}?mode=rw'.format(pathname2url(DB_NAME))
		dbConn = sqlite3.connect(dburi, uri=True)
	except sqlite3.OperationalError:
		print('DB does not exist, creating {}...'.format(DB_NAME))
		dbConn = sqlite3.connect(DB_NAME)
		create_db()
	print('Done.')


	loop = asyncio.get_event_loop()
	print('Radio initialization...')
	# Initialize the radio as a resource.
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
		radio._writeReg(REG_RSSITHRESH, 240)
		print('Radio settings: NODE = {0}, NETWORK = {1}'.format(NODE, NET))
		print('Listening...')

		loop.create_task(receiver(radio))
		loop.run_forever()

except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
	print('Keyboard interrupt.')

except Exception:
	e = sys.exc_info()[0]
	print('An error occured: {0}'.format(str(e)))

finally:
	print('Shutting down.')
	dbConn.close()
	loop.close()
