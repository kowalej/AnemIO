import datetime
import time

import django.conf as conf
import django.db.models as models
import django.utils as utils
import pytz
from rest_framework import serializers

class UnixDateTimeField(serializers.DateTimeField):
    description = "Unix timestamp integer to datetime object"

    def __init__(self, assume_milliseconds=False, format=serializers.empty, input_formats=None, default_timezone=None, *args, **kwargs):
        self.assume_milliseconds = assume_milliseconds
        super().__init__(format, input_formats, default_timezone, *args, **kwargs)

    def _is_string(self, val):
        try:
            if isinstance(val, unicode):  # Python 2.7 compatibility support
                return True
        except:
            pass
        return isinstance(val, str)

    def to_internal_value(self, val):
        if val is None or isinstance(val, datetime.datetime):
            return val
        if isinstance(val, datetime.date):
            return datetime.datetime(val.year, val.month, val.day)
        elif self._is_string(val):
            return utils.dateparse.parse_datetime(val)
        else:
            milliseconds = 0
            val = str(val)
            if self.assume_milliseconds == True and len(val) == 13:
                milliseconds = int(val[-3:])
                unix_timestamp = float(val[0:-3])
            else:
                unix_timestamp = int(val)
                milliseconds = 0
            datetime_value = datetime.datetime.utcfromtimestamp(unix_timestamp)
            datetime_value += datetime.timedelta(milliseconds=milliseconds)
            if conf.settings.USE_TZ:
                return utils.timezone.make_aware(datetime_value, timezone=pytz.utc)
            else:
                return datetime_value

    def to_representation(self, val):
        if val is None:
            if self.default == models.fields.NOT_PROVIDED: return None
            return self.default
        epoch = datetime.datetime.utcfromtimestamp(0)
        if val.tzinfo is not None:
            val = val.astimezone(pytz.utc)
            epoch = utils.timezone.make_aware(epoch, timezone=pytz.utc)
        delta = val - epoch
        if self.assume_milliseconds:
            return int(delta.total_seconds() * 1000)
        return int(delta.total_seconds())
