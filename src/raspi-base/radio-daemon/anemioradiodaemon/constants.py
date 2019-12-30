from enum import Enum
import os

DEFAULT_DB_NAME = 'anemio.db'

# Average time it takes to receive data after it was sent by station.
DEFAULT_RADIO_DELAY_MS = 20 

# Sleep time will be 50 ms (try get new set of packets every 50 ms).
DEFAULT_RECEIVE_SLEEP_SEC = 50.0/1000.0

NODE = 87 # Radio node number.
NET = 223 # Radio network.
ENCRYPT_KEY= os.environ.get('ANEMIO_ENCRYPT_KEY', 'J53Y25U5D8CE79NO') # tries to get from environment first.

COMPACT_MESSAGES_START = "^^"
COMPACT_MESSAGES_END = "$$"

class Devices(Enum):
    AMBIENT_LIGHT = 0
    COMPASS_ACCELEROMETER = 1
    PRESSURE = 2
    RAIN = 3
    TEMPERATURE_HUMIDITY = 4
    WATER_TEMPERATURE = 5
    WIND_DIRECTION = 6
    WIND_SPEED = 7
    TOTAL = 8

class Sensors(Enum):
    AMBIENT_LIGHT = 0
    COMPASS = 1
    ACCELEROMETER = 2
    PRESSURE = 3
    RAIN = 4
    TEMPERATURE = 5
    HUMIDITY = 6
    WATER_TEMPERATURE = 7
    WIND_DIRECTION = 8
    WIND_SPEED = 9
    TOTAL = 10

class RadioCommands(Enum):
    SETUP_START = 1
    REPORT_SETUP_STATE = 2
    SETUP_FINISH = 3
    REPORT_ONLINE_STATE = 4
    SAMPLES_START = 5
    SAMPLE_GROUP_DIVIDER = 6
    SAMPLE_WRITE = 7
    SAMPLES_FINISH = 8

class StationState(Enum):
    BOOTING = 1  # Station is booting up.
    ONLINE = 2  # Normal operation.
    PENDING_RESTART = 3  # Restart requested, awaiting completion.
    STANDBY = 4  # Save power - station will check for command once and awhile.
    OFFLINE = 5  # Station is off - in transit or otherwise.
