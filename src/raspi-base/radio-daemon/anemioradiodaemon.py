#!/usr/bin/env python3

from RFM69 import Radio, FREQ_915MHZ
from RFM69.registers import REG_RSSITHRESH
import datetime
import asyncio
from aiohttp import ClientSession
import time
import sqlite3
import sys
from enum import Enum

# Set high priority.
import os
os.nice(1)

class RadioCommands(Enum):
	SETUP_START = 1,
	SETUP_FINISH = 2,
	REPORT_ONLINE_STATE = 3,
	REPORT_SETUP_STATE = 4,
	SAMPLES_START = 5,
	SAMPLE_DEVICE_GROUP_START = 6,
	SAMPLE_WRITE = 7,
	SAMPLE_DEVICE_GROUP_END = 8,
	SAMPLES_FINISH = 9

NODE = 87 # Radio node number.
NET = 223 # Radio network.
TOSLEEP = 50.0/1000.0 # Sleep time will be 50 ms (try get new set of packets every 50 ms).
ENCRYPT_KEY= 'J53Y25U5D8CE79NO' # uncomment if we want to use encryption.

# DB setup.
print('Connecting to database...')
dbConn = sqlite3.connect('anemio.db')

async def receiver(radio):
	while True:
		for packet in radio.get_packets():
			print("Packet received:", packet)
			print(''.join([chr(letter) for letter in packet.data]))

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
