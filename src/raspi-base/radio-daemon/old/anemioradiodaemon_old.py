#!/usr/bin/env python3

from RFM69 import Radio, FREQ_915MHZ
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
	SETUP_START = 2,
	SETUP_FINISH = 4,
	REPORT_ONLINE_STATE = 8,
	REPORT_SETUP_STATE = 16,
	SAMPLES_START = 32,
	SAMPLE_WRITE = 64,
	SAMPLES_FINISH = 128

NODE = 87 # Radio node number.
NET = 223 # Radio network.
TOSLEEP = 7 # Sleep time will be 7 seconds (try get packets every 7 seconds).
# ENCRYPT_KEY= 'J53Y25U5D8CE79NO' # uncomment if we want to use encryption.

# DB setup.
print('Connecting to database...')
dbConn = sqlite3.connect('anemio.db')

async def receiver(radio):
	while True:    
		for packet in radio.get_packets():
			print("Packet received", packet)
			print(''.join([chr(letter) for letter in packet.data]))
			# await call_API("http://httpbin.org/post", packet)
		await asyncio.sleep(TOSLEEP)

try:
	loop = asyncio.get_event_loop()
	with Radio(FREQ_915MHZ, NODE, NET, isHighPower = True) as radio:
		print('Radio initialization...')
		print('Radio settings: NODE = {0}, NETWORK = {1}'.format(NODE, NET))
		print('Listening...')
		sequence = 0

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
