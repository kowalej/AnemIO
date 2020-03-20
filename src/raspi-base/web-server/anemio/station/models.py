from django.db import models

class AccelerometerXyz(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    x = models.FloatField(blank=True, null=True)
    y = models.FloatField(blank=True, null=True)
    z = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'accelerometer_xyz'


class AmbientLightState(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    state = models.TextField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'ambient_light_state'


class AmbientLightValues(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'ambient_light_values'


class CompassHeading(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    heading = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'compass_heading'


class CompassXyz(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    x = models.FloatField(blank=True, null=True)
    y = models.FloatField(blank=True, null=True)
    z = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'compass_xyz'


class DeviceState(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    online_devices = models.TextField(blank=True, null=True)
    offline_devices = models.TextField(blank=True, null=True)
    is_setup = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'device_state'


class Humidity(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'humidity'


class PressureAltitude(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_altitude'


class PressurePressure(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_pressure'


class PressureTemperature(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_temperature'


class PressureValues(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'pressure_values'


class RainState(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    state = models.TextField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'rain_state'


class RainValues(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'rain_values'


class StationLocation(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    lat = models.FloatField(blank=True, null=True)
    lon = models.FloatField(blank=True, null=True)
    is_actual = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'station_location'


class StationState(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    state = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'station_state'


class Temperature(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'temperature'


class WaterTemperature(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'water_temperature'


class WindDirection(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.IntegerField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'wind_direction'


class WindSpeed(models.Model):
    timestamp = models.IntegerField(blank=True, null=True)
    value = models.FloatField(blank=True, null=True)
    temperature = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'wind_speed'
