import datetime

import django.conf as conf
import django.db.models as models
import django.utils as utils
import pytz


class UnixDateTimeField(models.DateTimeField):
    description = "Unix timestamp integer to datetime object"

    def __init__(self, assume_milliseconds=False, *args, **kwargs):
        self.assume_milliseconds = assume_milliseconds
        super().__init__(*args, **kwargs)

    def deconstruct(self):
        name, path, args, kwargs = super().deconstruct()
        # Only include kwarg if it's not the default
        if self.assume_milliseconds is not False:
            kwargs['assume_milliseconds'] = self.assume_milliseconds
        return name, path, args, kwargs

    def get_internal_type(self):
        return 'PositiveIntegerField'

    def _is_string(self, val):
        try:
            if isinstance(val, unicode):  # Python 2.7 compatibility support
                return True
        except Exception:
            pass
        return isinstance(val, str)

    def to_python(self, val):
        if val is None or isinstance(val, datetime.datetime):
            return val
        if isinstance(val, datetime.date):
            return datetime.datetime(val.year, val.month, val.day)
        elif self._is_string(val):
            return utils.dateparse.parse_datetime(val)
        else:
            milliseconds = 0
            val = str(val)
            if self.assume_milliseconds is True and len(val) == 13:
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

    def get_db_prep_value(self, val, *args, **kwargs):
        if val is None:
            if self.default == models.fields.NOT_PROVIDED:
                return None
            return self.default
        epoch = datetime.datetime.utcfromtimestamp(0)
        if val.tzinfo is not None:
            val = val.astimezone(pytz.utc)
            epoch = utils.timezone.make_aware(epoch, timezone=pytz.utc)
        delta = val - epoch
        if self.assume_milliseconds:
            return int(delta.total_seconds() * 1000)
        return int(delta.total_seconds())

    def value_to_string(self, obj):
        val = self._get_val_from_obj(obj)
        return self.to_python(val).strftime(conf.settings.DATETIME_FORMAT)

    def from_db_value(self, val, *args, **kwargs):
        return self.to_python(val)
