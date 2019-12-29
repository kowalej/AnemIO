import sys
import unittest
from unittest.mock import MagicMock

import asyncio

# Mock the dependent modules for RFM69.
sys.modules['spidev'] = MagicMock()
sys.modules['RPi'] = MagicMock()
sys.modules['RPi.GPIO'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()
sys.modules['rpidevmocks'] = MagicMock()

from RFM69 import Radio, FREQ_915MHZ, Packet
from anemioradiodaemon import anemioradiodaemon

class TestingRadio(Radio):
    def __init__(self, freqBand, nodeID, networkID=100, **kwargs):
          pass
    
    def __enter__(self):
        pass

class TestAnemioRadioDaemon(unittest.TestCase):
    def test_startup(self):
        try:
            # Get our database.
            db_conn = anemioradiodaemon.setup_db('anemio-test.db')
            anemioradiodaemon.db_conn = db_conn

            # Get our async loop object.
            loop = asyncio.get_event_loop()

            # Initialize the radio as a resource.
            print('Radio initializing...')
            
            radio = TestingRadio(FREQ_915MHZ, 1, 1)
            print('Done.')

            packets = [
                Packet(int(1), int(1), int(-44), [91, 52, 93, 68, 58, 50, 79, 58, 49, 32, 45, 32, 80, 82, 69, 83, 83, 85, 82, 69, 32, 115, 101, 116, 117, 112, 32, 119, 97, 115, 32, 115, 117, 99, 99, 101, 115, 115, 102, 117, 108, 46])
            ]
            radio.get_packets = MagicMock(return_value=packets)
            anemioradiodaemon.start_listening(radio, loop)

        except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
            print('Keyboard interrupt.')

        finally:
            print('Shutting down.')
            db_conn.close()
            loop.close()

if __name__ == '__main__':
    unittest.main()
