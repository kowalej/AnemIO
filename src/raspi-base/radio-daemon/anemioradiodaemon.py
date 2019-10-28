#!/usr/bin/env python2

from RFM69 import RFM69
from RFM69.RFM69registers import RF69_915MHZ
import datetime
import time
import sqlite3
import sys

NODE = 8 # Radio node number.
NET = 223 # Radio network.
TIMEOUT = 11 # Timeout (seconds) - this is a bit less then double the time between transmits (6 seconds).
TOSLEEP = 0.1 # Sleep time will be 0.002 seconds = 2 ms.
# KEY='1234567891011121' # uncomment if we want to use encryption.

# DB setup.
print('Connecting to database.')
dbConn = sqlite3.connect('anemio.db')

radio = RFM69.RFM69(RF69_915MHZ, NODE, NET, isRFM69HW=False)
print('Radio initialization: ...')
print('Radio settings: NODE = {0}, NETWORK = {1}'.format(NODE, NET))

print('Performing rcCalibration.')
radio.rcCalibration()
print('Done calibration.')

radio.setHighPower(True)
print('Set radio to high power.')

#radio.encrypt(KEY) # uncomment if we want to use encryption.
# print('Enabled radio encryption.')

print('Radio temperature: {0}.'.format(radio.readTemperature(0)))

sequence=0
print('Listening...')
try:
	while True:
		radio.receiveBegin()
		timedOut=0
		while not radio.receiveDone():
			timedOut+=TOSLEEP
			time.sleep(TOSLEEP)
			if timedOut > TIMEOUT:
				print('Timed out waiting for signal from station.')
				break

		print('Message: {0} from {1} RSSI:{2} '.format(''.join([chr(letter) for letter in radio.DATA]), radio.SENDERID, radio.RSSI))

		if radio.ACKRequested():
			radio.sendACK()
		else:
			print('ACK not requested.')

except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
	print('Keyboard interrupt.')

except Exception:
	e = sys.exc_info()[0]
	print('An error occured: {0}'.format(str(e)))

finally:
	print('Shutting down.')
	radio.shutdown()
	dbConn.close()
