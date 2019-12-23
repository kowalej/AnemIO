#!/usr/bin/env python3

from RFM69 import Radio, FREQ_915MHZ
from RFM69.registers import REG_RSSITHRESH
import datetime
import asyncio
from aiohttp import ClientSession
import time
import sqlite3
import sys
import re
from constants import *

# Set high priority.
import os
os.nice(1)

NODE = 87 # Radio node number.
NET = 223 # Radio network.
TOSLEEP = 50.0/1000.0 # Sleep time will be 50 ms (try get new set of packets every 50 ms).
ENCRYPT_KEY= 'J53Y25U5D8CE79NO' # uncomment if we want to use encryption.

# DB setup.
print('Connecting to database...')
dbConn = sqlite3.connect('anemio.db')


# Handle one or more messages in packet (multiple occurs with compact send).
def handle_messages(data):
	messages = []
	breaks = [m.span() for m in re.finditer('\[[0-9]+\]', data)]
	for i in range(0, len(breaks)):
		radio_command = data[breaks[i][0]:breaks[i][1]]
		end_index = breaks[i+1][0] if i < len(breaks) - 1 else len(data)
		contents = data[breaks[i][1]:end_index]
		messages.append({ 'radio_command': radio_command, 'contents': contents })
		print(messages[-1])
	return messages

async def receiver(radio):
	compact_collecting = False
	messages = ''
	parsed_messages = []

	while True:	
		for packet in radio.get_packets():
			data = ''.join([chr(letter) for letter in packet.data])
			print('Packet received:', packet)
			#print(data)

			# See if we have started a long series of messages (compact messaging).
			if(data.startswith(COMPACT_MESSAGES_START)):
				compact_collecting = True
				data = data.lstrip(COMPACT_MESSAGES_START)
			if(data.endswith(COMPACT_MESSAGES_END)):
				compact_collecting = False
				data = data.rstrip(COMPACT_MESSAGES_END)

			# Always add the data to our messages object.
			messages += data

			if not compact_collecting:
				parsed_messages = handle_messages(messages)
				messages = '' # Make sure we clear messages now that we have parsed them.

		# TODO: Save data...
	 	# await call_API("http://httpbin.org/post", packet)
		await asyncio.sleep(TOSLEEP)

try:
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
