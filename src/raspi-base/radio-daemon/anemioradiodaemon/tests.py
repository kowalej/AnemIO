import asyncio
import datetime
import logging
import sys
import unittest
from unittest.mock import MagicMock
from logging.handlers import SMTPHandler
from pathlib import Path
from asyncio import CancelledError

# Mock the dependent modules for RFM69.
sys.modules['spidev'] = MagicMock()
sys.modules['RPi'] = MagicMock()
sys.modules['RPi.GPIO'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()
from RFM69 import FREQ_915MHZ, Packet, Radio

from .constants import *
from .radiodaemon import RadioDaemon, connect_db, get_ts

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
        raise CancelledError

class TestAnemioRadioDaemon(unittest.TestCase):
    def setUp(self):
        logging.basicConfig(level=logging.INFO, format='%(message)s')
        logger = logging.getLogger('anemio-test')
        logger.setLevel(logging.INFO)
        # Get our database.
        self.db_conn = connect_db(logger=logger, db_name='anemio-test.db')

    def run_daemon(self, packet_gen = None, send_return = True):
        radio = TestingRadio(FREQ_915MHZ, 1, 1)
        radio.get_packets = MagicMock(side_effect=packet_gen)
        radio.send = MagicMock(return_value=send_return)
        radio_daemon = RadioDaemon(self.db_conn, radio, receive_sleep_sec = 0.5)
        radio_daemon.run()

    def test_startup(self):
        # Send a series of startup packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]T:13432 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:1O:0 - COMPASS_ACCELEROMETER setup was unsuccessful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:1 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)

        self.start_timestamp = get_ts(datetime.datetime.utcnow(), DEFAULT_RADIO_DELAY_MS)
        self.run_daemon(packet_gen_local)
        self.end_timestamp = get_ts(datetime.datetime.utcnow())

        c = self.db_conn.cursor()

        # Ensure that station_location was automatically dirived from IP address of this base station.
        c.execute('SELECT * FROM station_location ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_location = c.fetchone()
        if(station_location is not None):
            self.assertGreaterEqual(station_location[0], self.start_timestamp)
            self.assertLessEqual(station_location[0], self.end_timestamp)
            self.assertIsNotNone(station_location[1])
            self.assertIsNotNone(station_location[2])
            self.assertEqual(station_location[3], 0)

        # Check that current station_state is online, and previous state was booting.
        c.execute('SELECT timestamp, state FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 2')
        station_state = c.fetchall()
        self.assertGreaterEqual(station_state[0][0], self.start_timestamp)
        self.assertLessEqual(station_state[0][0], self.end_timestamp)
        self.assertEqual(station_state[1][1], StationState.SETTING_UP.value)
        self.assertEqual(station_state[0][1], StationState.ONLINE.value)

        # Check the various device states.
        c.execute('SELECT * FROM device_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        device_state = c.fetchone()
        self.assertGreaterEqual(device_state[0], self.start_timestamp)
        self.assertLessEqual(device_state[0], self.end_timestamp)
        self.assertEqual(eval(device_state[1]), [0])
        self.assertEqual(eval(device_state[2]), [1])
        self.assertEqual(device_state[3], 1)

    # def test_station_restart(self):
    #     c = self.db_conn.cursor()

    #     # Write to DB to request restart.
    #     c.execute(
    #         'INSERT INTO station_state(timestamp, state) VALUES(?,?)', 
    #         (get_ts(datetime.datetime.utcnow()), StationState.PENDING_RESTART.value)
    #     )
    #     self.db_conn.commit()

    #     self.start_timestamp = get_ts(datetime.datetime.utcnow(), DEFAULT_RADIO_DELAY_MS)
    #     self.run_daemon(send_return=True)
    #     self.end_timestamp = get_ts(datetime.datetime.utcnow())

    def tearDown(self):
        self.db_conn.close()

if __name__ == '__main__':
    unittest.main()
