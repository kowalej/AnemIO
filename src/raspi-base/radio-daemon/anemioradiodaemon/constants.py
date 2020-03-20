from enum import Enum
import os

DEFAULT_DB_NAME = 'anemio.db'

# Average time it takes to receive data after it was sent by station.
DEFAULT_RADIO_DELAY_MS = 20 

# Sleep time will be 50 ms (try to send command or get new set of packets every 50 ms).
DEFAULT_TRANSRECEIVE_SLEEP_SEC = 50.0/1000.0

RADIO_STATION_NODE_ID = 97 # Station node number.
RADIO_BASE_NODE_ID = 87 # Base station node number.
NET = 223 # Radio network.
ENCRYPT_KEY= os.getenv('ANEMIO_ENCRYPT_KEY', 'J53Y25U5D8CE79NO') # tries to get from environment first.

# File log parameters.
LOG_FILE_MAX_SIZE_MB = os.getenv('ANEMIO_LOG_FILE_MAX_SIZE_MB', 5)
LOG_FILE_BACKUP_COUNT = os.getenv('ANEMIO_LOG_FILE_BACKUP_COUNT', 2)

# Email parameters.
EMAIL_FROM = os.getenv('ANEMIO_EMAIL_FROM', '')
EMAIL_TO = os.getenv('ANEMIO_EMAIL_TO', '')
EMAIL_HOST = os.getenv('ANEMIO_EMAIL_HOST', '')
EMAIL_PORT = os.getenv('ANEMIO_EMAIL_PORT', '')
EMAIL_USER = os.getenv('ANEMIO_EMAIL_USER', '')
EMAIL_PASSWORD = os.getenv('ANEMIO_EMAIL_PASSWORD', '')

# Rate limiting.
EMAIL_RATE = os.getenv('ANEMIO_EMAIL_RATE', '')
EMAIL_RATE_SECONDS = os.getenv('ANEMIO_EMAIL_RATE_SECONDS', '')
EMAIL_RATE_BURST = os.getenv('ANEMIO_EMAIL_RATE_BURST', '')

COMPACT_MESSAGES_START = "^^"
COMPACT_MESSAGES_END = "$$"

# Maximum allowed time to receive no radio messages, until reporting that we are in unreachable state.
MAX_NO_RECEIVE_SECONDS = 5

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

class SensorCategory(Enum):
    AMBIENT_LIGHT_VALUES = 0
    AMBIENT_LIGHT_STATE = 1
    COMPASS_XYZ = 2
    COMPASS_HEADING = 3
    ACCELEROMETER_XYZ = 4
    PRESSURE_PRESSURE = 5
    PRESSURE_TEMPERATURE = 6
    PRESSURE_ALTITUDE = 7
    RAIN_VALUES = 8
    RAIN_STATE = 9
    TEMPERATURE = 10
    HUMIDITY = 11
    WATER_TEMPERATURE = 12
    WIND_DIRECTION = 13
    WIND_SPEED = 14

class RadioCommands(Enum):
    SETUP_START = 1
    REPORT_SETUP_STATE = 2
    SETUP_FINISH = 3
    REPORT_ONLINE_STATE = 4
    SAMPLES_START = 5
    SAMPLE_GROUP_DIVIDER = 6
    SAMPLE_WRITE = 7
    SAMPLES_FINISH = 8
    SLEEP = 9
    WAKE = 10
    RESTART = 11

class StationState(Enum):
    UNKNOWN = -1  # Damemon started, cannot determine state.
    OFFLINE = 0  # Station is off - intentionally in transit or otherwise.
    SETUP_BOOT = 1  # Station is setting up (initial launch).
    ONLINE = 2  # Normal operation.
    RESTART_REQUESTED = 3  # Restart requested, awaiting completion, will transition to RESTARTING, then SETUP_BOOT / ONLINE.
    RESTARTING = 4  # Station acknowledged restart request, now we are awaiting it.
    SLEEP_REQUESTED = 5  # Sleep mode requested, awaiting completion, will transition to sleeping.
    SLEEPING = 6  # Save power - station will check for command once and awhile.
    WAKE_REQUESTED = 7  # Wake requested, awaiting completion, will transition to online.
    SETUP_WAKE = 8  # Station is setting up (from boot).
    UNREACHABLE = 9 # No signal has been recieved for awhile.
    
