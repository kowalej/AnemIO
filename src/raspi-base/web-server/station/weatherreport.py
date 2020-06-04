from datetime import datetime, timedelta
import station.models as models
import pandas as pd
from station.constants import REPORT_INSTANT_TIME_MS
import logging

logger = logging.getLogger(__name__)


class WeatherReport():
    # Generate a report from start_date_time to end_date_time, with precision (timestep) of given period.
    # Period can be one of 'i', 'm', 'h', 'd', 'w', 'M'. Instant, minute, hour, day, week, month.
    # Instant means the smallest possible timestep (basically how often does the station send data, ~5 seconds).
    # For example if you input 01-01-2019 as start_date_time and 12-31-2019 with period as 'd', you will
    # recieve a series of daily values included highs, lows, and averages for various metrics for all of 2019.
    # Note that calendar dates are considered, so if you use a start_date_time that falls midweek, and want weekly
    # timesteps, the dates will start at the nearest full week starting on Sunday.
    def __init__(self, start_date_time: datetime, end_date_time: datetime, period='i', use_aggregate=False):
        self.end_date_time = end_date_time
        self.start_date_time = start_date_time
        self.period = period
        self.date_delta = end_date_time - start_date_time

        if period not in ['i', 'm', 'h', 'd', 'w', 'M']:
            raise Exception('Unsupported period: please use one of i, m, h, d, w, M.')

        if use_aggregate is False:
            self.calc_from_raw()
        else:
            self.calc_from_aggregate()

    def calc_from_raw(self):
        # Accelerometer XYZ.
        self.df_AccelerometerXyz = pd.DataFrame(
            list(
                models.AccelerometerXyz
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_AccelerometerXyz) > 0:
            self.df_AccelerometerXyz.set_index('timestamp', inplace=True)
            self.df_AccelerometerXyz.drop('ROWID', axis=1, inplace=True)

        # Ambient light state.
        self.df_AmbientLightState = pd.DataFrame(
            list(
                models.AmbientLightState
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_AmbientLightState) > 0:
            self.df_AmbientLightState.set_index('timestamp', inplace=True)
            self.df_AmbientLightState.drop('ROWID', axis=1, inplace=True)

        # Ambient light values.
        self.df_AmbientLightValues = pd.DataFrame(
            list(
                models.AmbientLightValues
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_AmbientLightValues) > 0:
            self.df_AmbientLightValues.set_index('timestamp', inplace=True)
            self.df_AmbientLightValues.drop('ROWID', axis=1, inplace=True)

        # Compass heading.
        self.df_CompassHeading = pd.DataFrame(
            list(
                models.CompassHeading
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_CompassHeading) > 0:
            self.df_CompassHeading.set_index('timestamp', inplace=True)
            self.df_CompassHeading.drop('ROWID', axis=1, inplace=True)

        # Compass XYZ.
        self.df_CompassXyz = pd.DataFrame(
            list(
                models.CompassXyz
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_CompassXyz) > 0:
            self.df_CompassXyz.set_index('timestamp', inplace=True)
            self.df_CompassXyz.drop('ROWID', axis=1, inplace=True)

        # Humidity.
        self.df_Humidity = pd.DataFrame(
            list(
                models.Humidity
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_Humidity) > 0:
            self.df_Humidity.set_index('timestamp', inplace=True)
            self.df_Humidity.drop('ROWID', axis=1, inplace=True)

        # Altitude (from pressure).
        self.df_PressureAltitude = pd.DataFrame(
            list(
                models.PressureAltitude
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_PressureAltitude) > 0:
            self.df_PressureAltitude.set_index('timestamp', inplace=True)
            self.df_PressureAltitude.drop('ROWID', axis=1, inplace=True)

        # Pressure.
        self.df_PressurePressure = pd.DataFrame(
            list(
                models.PressurePressure
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_PressurePressure) > 0:
            self.df_PressurePressure.set_index('timestamp', inplace=True)
            self.df_PressurePressure.drop('ROWID', axis=1, inplace=True)

        # Temperature (from pressure sensor / backup).
        self.df_PressureTemperature = pd.DataFrame(
            list(
                models.PressureTemperature
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_PressureTemperature) > 0:
            self.df_PressureTemperature.set_index('timestamp', inplace=True)
            self.df_PressureTemperature.drop('ROWID', axis=1, inplace=True)

        # Rain guage - n/a right now, may add later (5/16/2020).
        # self.df_RainGauge = pd.DataFrame(
        #     list(
        #         models.RainGauge
        #         .objects
        #         .filter(timestamp__gte=self.start_date_time)
        #         .filter(timestamp__lte=self.end_date_time)
        #         .values()
        #     )
        # )
        # if len(self.df_RainGauge) > 0:
        #     self.df_RainGauge.reindex(index='timestamp')
        #     self.df_RainGauge.drop('ROWID', axis=1, inplace=True)

        # Rain state (pre-computed from station).
        self.df_RainState = pd.DataFrame(
            list(
                models.RainState
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_RainState) > 0:
            self.df_RainState.set_index('timestamp', inplace=True)
            self.df_RainState.drop('ROWID', axis=1, inplace=True)

        # Rain values.
        self.df_RainValues = pd.DataFrame(
            list(
                models.RainValues
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_RainValues) > 0:
            self.df_RainValues.set_index('timestamp', inplace=True)
            self.df_RainValues.drop('ROWID', axis=1, inplace=True)

        # Temperature (primary sensor).
        self.df_Temperature = pd.DataFrame(
            list(
                models.Temperature
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_Temperature) > 0:
            self.df_Temperature.set_index('timestamp', inplace=True)
            self.df_Temperature.drop('ROWID', axis=1, inplace=True)

        # Water Temperature.
        self.df_WaterTemperature = pd.DataFrame(
            list(
                models.WaterTemperature
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_WaterTemperature) > 0:
            self.df_WaterTemperature.set_index('timestamp', inplace=True)
            self.df_WaterTemperature.drop('ROWID', axis=1, inplace=True)

        # Wind Direction.
        self.df_WindDirection = pd.DataFrame(
            list(
                models.WindDirection
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_WindDirection) > 0:
            self.df_WindDirection.set_index('timestamp', inplace=True)
            self.df_WindDirection.drop('ROWID', axis=1, inplace=True)

        # Wind Speed.
        self.df_WindSpeed = pd.DataFrame(
            list(
                models.WindSpeed
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        if len(self.df_WindSpeed) > 0:
            self.df_WindSpeed.set_index('timestamp', inplace=True)
            self.df_WindSpeed.drop('ROWID', axis=1, inplace=True)
