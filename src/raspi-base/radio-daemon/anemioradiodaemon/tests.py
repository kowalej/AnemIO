import sys
import unittest
from unittest.mock import MagicMock

import asyncio
import datetime

# Mock the dependent modules for RFM69.
sys.modules['spidev'] = MagicMock()
sys.modules['RPi'] = MagicMock()
sys.modules['RPi.GPIO'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()

from RFM69 import Radio, FREQ_915MHZ, Packet
from anemioradiodaemon import anemioradiodaemon
from anemioradiodaemon.constants import *

# Fake radio (based on actual Radio), we override some initialization methods.
class TestingRadio(Radio):
    def __init__(self, freqBand, nodeID, networkID=100, **kwargs):
          pass
    
    def __enter__(self):
        pass

packet_index = -1
def packet_gen(packets: list):
    global packet_index
    packet_index += 1
    try:
        packet = packets[packet_index]
        return [packet]
    except IndexError:
        return []

def run_radio(packet_gen):
    try:
        # Get our async loop object.
        loop = asyncio.get_event_loop()

        # Record station location estimate, based on our base station's IP address.
        anemioradiodaemon.record_location()

        # Initialize the radio as a resource.
        print('Radio initializing...')
        
        radio = TestingRadio(FREQ_915MHZ, 1, 1)
        print('Done.')

        radio.get_packets = MagicMock(side_effect=packet_gen)
        anemioradiodaemon.start_listening(radio, loop)

    except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
        print('Keyboard interrupt.')

    finally:
        print('Shutting down.')
        if loop is not None:
            loop.close()

class TestAnemioRadioDaemon(unittest.TestCase):
    def setUp(self):
        # Get our database.
        self.db_conn = anemioradiodaemon.setup_db('anemio-test.db')
        anemioradiodaemon.RECIEVE_SLEEP_SEC = 0.1
        anemioradiodaemon.db_conn = self.db_conn

    def test_startup(self):
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]T:13432 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:1O:0 - COMPASS_ACCELEROMETER setup was unsuccessful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:1 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)

        self.start_timestamp = anemioradiodaemon.get_ts(datetime.datetime.utcnow())
        run_radio(packet_gen_local)
        self.end_timestamp = anemioradiodaemon.get_ts(datetime.datetime.utcnow())

        c = anemioradiodaemon.db_conn.cursor()

        c.execute('SELECT * FROM station_location ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_location = c.fetchone()
        if(station_location is not None):
            self.assertGreaterEqual(station_location[0], self.start_timestamp)
            self.assertLessEqual(station_location[0], self.end_timestamp)
            self.assertIsNotNone(station_location[1])
            self.assertIsNotNone(station_location[2])
            self.assertEqual(station_location[3], 0)

        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_state = c.fetchone()
        self.assertGreaterEqual(station_state[0], self.start_timestamp)
        self.assertLessEqual(station_state[0], self.end_timestamp)
        self.assertEqual(station_state[1], StationState.ONLINE.value)

        c.execute('SELECT * FROM device_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        device_state = c.fetchone()
        self.assertGreaterEqual(device_state[0], self.start_timestamp)
        self.assertLessEqual(device_state[0], self.end_timestamp)
        self.assertEqual(eval(device_state[1]), [0])
        self.assertEqual(eval(device_state[2]), [1])
        self.assertEqual(device_state[3], 1)
    
    def tearDown(self):
        self.db_conn.close()

if __name__ == '__main__':
    unittest.main()
