from enum import Enum
import os


# Email parameters.
EMAIL_FROM = os.getenv('ANEMIO_EMAIL_FROM', '')
EMAIL_TO = os.getenv('ANEMIO_EMAIL_TO', '')
EMAIL_HOST = os.getenv('ANEMIO_EMAIL_HOST', '')
EMAIL_PORT = os.getenv('ANEMIO_EMAIL_PORT', '')
EMAIL_USER = os.getenv('ANEMIO_EMAIL_USER', '')
EMAIL_PASSWORD = os.getenv('ANEMIO_EMAIL_PASSWORD', '')

# How often does our station send data.
RADIO_SEND_INTERVAL_MS = 5000


class Devices(Enum):
    AMBIENT_LIGHT = 0
    BATTERY_INFO = 1
    COMPASS_ACCELEROMETER = 2
    PRESSURE = 3
    RAIN = 4
    TEMPERATURE_HUMIDITY = 5
    WATER_TEMPERATURE = 6
    WIND_DIRECTION = 7
    WIND_SPEED = 8
    TOTAL = 9


class SensorCategory(Enum):
    AMBIENT_LIGHT_VALUES = 0
    AMBIENT_LIGHT_STATE = 1
    BATTERY_LEVEL = 2
    COMPASS_XYZ = 3
    COMPASS_HEADING = 4
    ACCELEROMETER_XYZ = 5
    PRESSURE_PRESSURE = 6
    PRESSURE_TEMPERATURE = 7
    PRESSURE_ALTITUDE = 8
    RAIN_VALUES = 9
    RAIN_STATE = 10
    TEMPERATURE = 11
    HUMIDITY = 12
    WATER_TEMPERATURE = 13
    WIND_DIRECTION = 14
    WIND_SPEED = 15


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
    TOTAL = 12


class StationState(Enum):
    UNKNOWN = -1  # Damemon started, cannot determine state.
    OFFLINE = 0  # Station is off - intentionally in transit or otherwise.
    SETUP_BOOT = 1  # Station is setting up (initial launch).
    ONLINE = 2  # Normal operation.
    RESTART_REQUESTED = 3  # Restart requested, awaiting completion, will transition to RESTARTING, then SETUP_BOOT / ONLINE.
    RESTARTING = 4  # Station acknowledged restart request, now we are awaiting it.
    SLEEP_REQUESTED = 5  # Sleep mode requested, awaiting completion, will transition to SLEEPING.
    SLEEPING = 6  # Save power - station will check for command once and awhile.
    WAKE_REQUESTED = 7  # Wake requested, awaiting completion, will transition to ONLINE.
    SETUP_WAKE = 8  # Station is setting up (from boot).
    CALIBRATE_REQUESTED = 9  # Calibrate mode requested, awaiting completion, will transition to CALIBRATING.
    CALIBRATING = 10  # Station acknowledged calibration requested, now it is calibrating.
    UNREACHABLE = 11  # No signal has been recieved for awhile.
