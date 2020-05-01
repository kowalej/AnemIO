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

from RFM69 import FREQ_915MHZ, Packet, Radio  # noqa: E402

from .constants import *  # noqa: E402
from .radiodaemon import RadioDaemon, connect_db, get_ts  # noqa: E402


test_db_name = 'anemio-test.db'
packet_index = -1


# Fake radio (based on actual Radio), we override some initialization methods.
class TestingRadio(Radio):
    def __init__(self, freqBand, nodeID, networkID=100, **kwargs):
        pass

    def __enter__(self):
        pass


def packet_gen(packets: [Packet]):
    global packet_index
    packet_index += 1
    try:
        packet = packets[packet_index]
        packet.received = datetime.datetime.utcnow()
        return [packet]
    except IndexError:
        raise CancelledError


class TestAnemioRadioDaemon(unittest.TestCase):
    def setUp(self):
        global packet_index
        packet_index = -1
        logging.basicConfig(level=logging.INFO, format='%(message)s')
        logger = logging.getLogger('anemio-debug')
        logger.setLevel(logging.INFO)
        # Get our database.
        self.db_conn = connect_db(logger=logger, db_name=test_db_name)

    def run_daemon(self, packet_gen=None, send_return=True, is_initial=True):
        radio = TestingRadio(FREQ_915MHZ, 1, 1)
        radio.get_packets = MagicMock(side_effect=packet_gen)
        radio.send = MagicMock(return_value=send_return)
        if is_initial:
            self.radio_daemon = RadioDaemon(self.db_conn, radio, receive_sleep_sec=0.5)
        else:
            self.radio_daemon.radio = radio
        self.radio_daemon.run()

    def test_startup(self):
        # Send a series of startup packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]I:1 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:1O:0 - COMPASS_ACCELEROMETER setup was unsuccessful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:1 - setup complete with 1 devices offline.')))
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
        self.assertEqual(station_state[1][1], StationState.SETUP_BOOT.value)
        self.assertEqual(station_state[0][1], StationState.ONLINE.value)

        # Check the various device states.
        c.execute('SELECT * FROM device_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        device_state = c.fetchone()
        self.assertGreaterEqual(device_state[0], self.start_timestamp)
        self.assertLessEqual(device_state[0], self.end_timestamp)
        self.assertEqual(eval(device_state[1]), [0])
        self.assertEqual(eval(device_state[2]), [1])
        self.assertEqual(device_state[3], 1)

    def test_station_restart(self):
        global packet_index

        c = self.db_conn.cursor()

        # Send a series of startup packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]I:1 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:0 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)
        self.run_daemon(packet_gen=packet_gen_local)

        # Write to DB to request restart.
        c.execute(
            'INSERT INTO station_state(timestamp, state) VALUES(?,?)',
            (get_ts(datetime.datetime.utcnow()), StationState.RESTART_REQUESTED.value)
        )
        self.db_conn.commit()

        self.run_daemon(packet_gen=packet_gen_local, send_return=True, is_initial=False)

        # Check if in restarting state.
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_state = c.fetchone()
        self.assertEqual(station_state[1], StationState.RESTARTING.value)

        packet_index = -1
        self.run_daemon(packet_gen=packet_gen_local, is_initial=False)

        # Check if in it restarted (setup / online states).
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 2')
        station_state = c.fetchall()
        self.assertEqual(station_state[1][1], StationState.SETUP_BOOT.value)
        self.assertEqual(station_state[0][1], StationState.ONLINE.value)

    def test_sleep_wake(self):
        global packet_index

        c = self.db_conn.cursor()

        # Send a series of startup packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]I:1 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:0 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)
        self.run_daemon(packet_gen=packet_gen_local)

        # Write to DB to request sleep.
        c.execute(
            'INSERT INTO station_state(timestamp, state) VALUES(?,?)',
            (get_ts(datetime.datetime.utcnow()), StationState.SLEEP_REQUESTED.value)
        )
        self.db_conn.commit()

        self.run_daemon(packet_gen=packet_gen_local, send_return=True, is_initial=False)

        # Check if in restarting state.
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_state = c.fetchone()
        self.assertEqual(station_state[1], StationState.SLEEPING.value)

        # Write to DB to request wake.
        c.execute(
            'INSERT INTO station_state(timestamp, state) VALUES(?,?)',
            (get_ts(datetime.datetime.utcnow()), StationState.WAKE_REQUESTED.value)
        )
        self.db_conn.commit()

        packet_index = -1
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]I:0 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:0 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)
        self.run_daemon(packet_gen=packet_gen_local, is_initial=False)

        # Check if station awakened (setup / online states).
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 2')
        station_state = c.fetchall()
        self.assertEqual(station_state[1][1], StationState.SETUP_WAKE.value)
        self.assertEqual(station_state[0][1], StationState.ONLINE.value)

    def test_station_calibrate(self):
        global packet_index

        c = self.db_conn.cursor()

        # Send a series of startup packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[1]I:1 - Setup starting.'))),
            Packet(1, 1, 1, list(bytearray(b'[2]D:0O:1 - AMBIENT_LIGHT setup was successful.'))),
            Packet(1, 1, 1, list(bytearray(b'[3]O:1F:0 - setup complete with 0 devices offline.')))
        ]

        def packet_gen_local():
            return packet_gen(packets)
        self.run_daemon(packet_gen=packet_gen_local)

        # Write to DB to request restart.
        c.execute(
            'INSERT INTO station_state(timestamp, state) VALUES(?,?)',
            (get_ts(datetime.datetime.utcnow()), StationState.CALIBRATE_REQUESTED.value)
        )
        self.db_conn.commit()

        self.run_daemon(packet_gen=packet_gen_local, send_return=True, is_initial=False)

        # Check if in calibrating state.
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_state = c.fetchone()
        self.assertEqual(station_state[1], StationState.CALIBRATING.value)

        packet_index = -1
        self.run_daemon(packet_gen=packet_gen_local, is_initial=False)

        # Check if we're done calibrating (back online).
        c.execute('SELECT * FROM station_state ORDER BY timestamp DESC, ROWID DESC LIMIT 2')
        station_state = c.fetchall()
        self.assertEqual(station_state[0][1], StationState.ONLINE.value)

    def test_data_packets(self):
        global packet_index

        c = self.db_conn.cursor()

        # Send a series of data packets.
        packets = [
            Packet(1, 1, 1, list(bytearray(b'[5]T:225188'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:5F:Ti,Xr,Yr,ZrN:7B:3451R:-1.00'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,-0.75,0.04,-9.38|525,-0.78,0.08,-9.38|1045,-0.75,0.08,-9.22|1570,-0.75,0.04,-9.26|2106,-0.78,0.08,-9.30|2632,-0.67,0.08,-9.22|3168,-0.71,0.08,-9.22'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:0F:Ti,VrN:4B:3480R:241.25'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,776.80|1019,774.40|2033,774.20|3050,778.70'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:1F:Ti,VtN:1B:3467R:633.63'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,Partly Sunny'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:3F:Ti,Xr,Yr,ZrN:7B:3453R:674.31'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,11.09,30.73,72.86|526,10.91,30.64,72.55|1046,10.82,30.27,72.65|1571,11.09,30.64,73.06|2107,10.91,30.55,73.16|2633,11.00,30.64,73.06|3169,10.82,30.45,72.86'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:4F:Ti,ViN:7B:3452R:598.41'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,70|525,70|1045,70|1570,70|2106,70|2632,70|3168,70'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:6F:Ti,VrN:4B:3409R:428.71'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,100276.00|1045,100287.00|2106,100283.00|3168,100291.00'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:7F:Ti,VrN:4B:3409R:754.18'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,25.51|1045,25.50|2106,25.51|3168,25.51'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:8F:Ti,VrN:4B:3409R:886.59'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,87.66|1045,86.73|2107,87.07|3169,86.40'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:9F:Ti,VrN:7B:3380R:874.79'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,961.78|512,959.98|1045,960.68|1558,961.64|2105,961.50|2620,961.46|3167,961.66'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:10F:Ti,VtN:1B:3378R:987.20'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,fuck you fo sho'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:11F:Ti,VrN:4B:3362R:1003.60'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,25.80|1037,25.80|2102,25.80|3161,25.80'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:12F:Ti,VrN:4B:3362R:1012.30'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,40.20|1037,40.20|2102,40.20|3161,40.20'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:13F:Ti,VrN:1B:3333R:666.15'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,24.27'))),
            Packet(1, 1, 1, list(bytearray(b'[6]S:15F:Ti,Vr,TCrN:4B:3300R:842.57'))),
            Packet(1, 1, 1, list(bytearray(b'[7]0,1.10,55.41|1046,1.08,55.40|2093,1.08,55.41|3144,1.09,55.40'))),
            Packet(1, 1, 1, list(bytearray(b'[8]S:21/21T:239853R:930.79'))),
        ]

        def packet_gen_local():
            return packet_gen(packets)

        self.start_timestamp = get_ts(datetime.datetime.utcnow(), DEFAULT_RADIO_DELAY_MS)
        self.run_daemon(packet_gen_local)
        self.end_timestamp = get_ts(datetime.datetime.utcnow())

        c = self.db_conn.cursor()

        # Ensure that .
        c.execute('SELECT * FROM station_location ORDER BY timestamp DESC, ROWID DESC LIMIT 1')
        station_location = c.fetchone()
        if(station_location is not None):
            self.assertGreaterEqual(station_location[0], self.start_timestamp)
            self.assertLessEqual(station_location[0], self.end_timestamp)
            self.assertIsNotNone(station_location[1])
            self.assertIsNotNone(station_location[2])
            self.assertEqual(station_location[3], 0)

    def tearDown(self):
        self.db_conn.close()
        try:
            os.remove(test_db_name)


if __name__ == '__main__':  # noqa: E999
    unittest.main()
