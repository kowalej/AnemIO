from django.db import models
from .fields import UnixDateTimeField
from . import constants


class AccelerometerXyz(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    x = models.FloatField(blank=True, null=True)
    y = models.FloatField(blank=True, null=True)
    z = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'accelerometer_xyz'


class AmbientLightState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    state = models.TextField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'ambient_light_state'


class AmbientLightValues(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'ambient_light_values'


class BatteryLevel(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'battery_level'


class CompassHeading(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    heading = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'compass_heading'


class CompassXyz(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    x = models.FloatField(blank=True, null=True)
    y = models.FloatField(blank=True, null=True)
    z = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'compass_xyz'


class DeviceState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    online_devices = models.TextField(blank=True, null=True)
    offline_devices = models.TextField(blank=True, null=True)
    is_setup = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'device_state'


class Humidity(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'humidity'


class PressureAltitude(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_altitude'


class PressurePressure(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_pressure'


class PressureTemperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_temperature'


class RainState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    state = models.TextField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'rain_state'


class RainValues(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'rain_values'


class StationLocation(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    lat = models.FloatField(blank=True, null=True)
    lon = models.FloatField(blank=True, null=True)
    is_actual = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'station_location'


class StationState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID', null=False, blank=True)
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    state = models.IntegerField(blank=True, null=True, choices=[(tag, tag.value) for tag in constants.StationState])

    class Meta:
        managed = False
        db_table = 'station_state'


class Temperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'temperature'


class WaterTemperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'water_temperature'


class WindDirection(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'wind_direction'


class WindSpeed(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=True, null=True)
    temperature = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'wind_speed'
