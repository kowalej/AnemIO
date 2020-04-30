from datetime import datetime, timedelta
import station.models as models
import pandas as pd
from station.constants import RADIO_SEND_INTERVAL_MS


def get_num_periods(period, date_time_detla: timedelta):
    if period == 'i':
        return date_time_detla.total_seconds // (RADIO_SEND_INTERVAL_MS / 1000)
    elif period == 'm':
        return date_time_detla.total_seconds // 60
    elif period == 'h':
        return date_time_detla.total_seconds // (60*60)
    elif period == 'd':
        return date_time_detla.days
    elif period == 'w':
        return date_time_detla.days // 7


class WeatherReport():
    # Generate a report from start_date_time to end_date_time, with precision (timestep) of given period.
    # Period can be one of 'i', 'm', 'h', 'd', 'w', 'M'. Instant, minute, hour, day, week, month.
    # Instant means the smallest possible timestep (basically how often does the station send data, ~5 seconds).
    # For example if you input 01-01-2019 as start_date_time and 12-31-2019 with period as 'd', you will
    # recieve a series of daily values included highs, lows, and averages for various metrics for all of 2019.
    # Note that exact calendar dates are not considered, meaning that (for example) if your start_date_time
    # begins in the middle of a real calendar week, the measure of one week will start at that exact date time
    # and jump to the middle of the next real calendar week (i.e. increment 7 days from your start_date_time).
    def __init__(self, end_date_time: datetime.datetime, start_date_time: datetime.datetime, period='i'):
        self.end_date_time = end_date_time
        self.start_date_time = start_date_time
        self.period = period
        self.date_delta = end_date_time - start_date_time

        if period in ['i', 'm', 'h']:
            self.calc_instant()
        elif period in ['d', 'w', 'm']:
            self.calc_from_aggregate()
        else:
            raise Exception('Unsupported period: please use one of i, m, h, d, w, M.')

    def calc_from_raw(self):
        df_accelerometerXyz = pd.DataFrame(
            list(
                models.AccelerometerXyz
                .objects
                .filter(timestamp__gte=self.start_date_time)
                .filter(timestamp__lte=self.end_date_time)
                .values()
            )
        )
        df_accelerometerXyz.rolling(5, win_type='boxcar').average()

    # def calc_from_aggregate(self):
    #     if self.period == 'd':
