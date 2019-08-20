#!/usr/bin/env python2

from RFM69 import RFM69
from RFM69.RFM69registers import RF69_915MHZ
import datetime
import time

NODE=8
NET=100
#KEY='1234567891011121'
TIMEOUT=3
TOSLEEP=0.1

radio = RFM69.RFM69(RF69_915MHZ, NODE, NET, True)
print('class initialized')

print('reading all registers')
results = radio.readAllRegs()
#for result in results:
#    print result

print('Performing rcCalibration')
radio.rcCalibration()

print('setting high power')
radio.setHighPower(True)

print('Checking temperature')
print(radio.readTemperature(0))

# No encryption.
#print('setting encryption')
#radio.encrypt(KEY)

sequence=0
print('starting loop...')
while True:


    msg = f'I\'m radio {NODE}: {sequence}'
    sequence = sequence + 1

    print(f'tx to radio {radio.SENDERID}: {msg}')
    if radio.sendWithRetry(1, msg, 3, 20):
        print('ack recieved')

    print('starting recv...')
    radio.receiveBegin()
    timedOut=0
    while not radio.receiveDone():
        timedOut+=TOSLEEP
        time.sleep(TOSLEEP)
        if timedOut > TIMEOUT:
            print('timed out waiting for recv')
            break

    print('end recv...')
    print(' ### %s from %s RSSI:%s ' % (''.join([chr(letter) for letter in radio.DATA]), radio.SENDERID, radio.RSSI))

    if radio.ACKRequested():
        radio.sendACK()
    else:
        print('ack not requested...')

print('shutting down')
radio.shutdown()
