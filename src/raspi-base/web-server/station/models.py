from django.db import models
from .fields import UnixDateTimeField
from . import constants


class AccelerometerXyz(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    x = models.FloatField(blank=False, null=False)
    y = models.FloatField(blank=False, null=False)
    z = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'accelerometer_xyz'


class AmbientLightState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.TextField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'ambient_light_state'


class AmbientLightValues(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'ambient_light_values'


class BatteryLevel(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'battery_level'


class CompassHeading(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.IntegerField(blank=False, null=False)

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
    online_devices = models.TextField(blank=True, null=False)
    offline_devices = models.TextField(blank=True, null=False)
    is_setup = models.IntegerField(blank=True, null=False)

    class Meta:
        managed = False
        db_table = 'device_state'


class Humidity(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'humidity'


class PressureAltitude(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'pressure_altitude'


class PressurePressure(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'pressure_pressure'


class PressureTemperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'pressure_temperature'


class RainGauge(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = True
        db_table = 'rain_gauge'


class RainState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.TextField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'rain_state'


class RainValues(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'rain_values'


class StationLocation(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    lat = models.FloatField(blank=False, null=False)
    lon = models.FloatField(blank=False, null=False)
    is_actual = models.IntegerField(blank=True, null=False)

    class Meta:
        managed = False
        db_table = 'station_location'


class StationState(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID', null=False, blank=True)
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    state = models.IntegerField(blank=False, null=False, choices=[(tag, tag.value) for tag in constants.StationState])

    class Meta:
        managed = False
        db_table = 'station_state'


class Temperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'temperature'


class WaterTemperature(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'water_temperature'


class WeatherReport(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.CharField(blank=False, null=False, max_length=500)
    unit = models.CharField(blank=False, null=False, max_length=15)

    class Meta:
        managed = True
        db_table = 'weather_report'


class WindDirection(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.IntegerField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'wind_direction'


class WindSpeed(models.Model):
    id = models.IntegerField(primary_key=True, name='ROWID')
    timestamp = UnixDateTimeField(assume_milliseconds=True, blank=False, null=False)
    value = models.FloatField(blank=False, null=False)
    tc = models.FloatField(blank=False, null=False)

    class Meta:
        managed = False
        db_table = 'wind_speed'
