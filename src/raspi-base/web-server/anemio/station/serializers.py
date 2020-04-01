from rest_framework import serializers
import station.models as models
from station.serializerfields import UnixDateTimeField


timestamp_serializer = UnixDateTimeField(assume_milliseconds=True)
timestamp_dt_serialize = serializers.DateTimeField(source='timestamp')


class AccelerometerXyzSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.AccelerometerXyz
        fields = '__all__'


class AmbientLightStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.AmbientLightState
        fields = '__all__'


class AmbientLightValuesSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.AmbientLightValues
        fields = '__all__'


class BatteryLevelSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.BatteryLevel
        fields = '__all__'


class CompassHeadingSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.CompassHeading
        fields = '__all__'


class CompassXyzSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.CompassXyz
        fields = '__all__'


class DeviceStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize
    online_devices = serializers.SerializerMethodField('get_online_devices')
    offline_devices = serializers.SerializerMethodField('get_offline_devices')

    def get_online_devices(self, obj):
        online_devices = [int(x) for x in obj.online_devices.replace('[', '').replace(']', '').split(',') if len(x) > 0]
        return online_devices

    def get_offline_devices(self, obj):
        offline_devices = [int(x) for x in obj.offline_devices.replace('[', '').replace(']', '').split(',') if len(x) > 0]
        return offline_devices

    class Meta:
        model = models.DeviceState
        fields = '__all__'


class HumiditySerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.Humidity
        fields = '__all__'


class PressureAltitudeSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.PressureAltitude
        fields = '__all__'


class PressurePressureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.PressurePressure
        fields = '__all__'


class PressureTemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.PressureTemperature
        fields = '__all__'


class RainStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.RainState
        fields = '__all__'


class RainValuesSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.RainValues
        fields = '__all__'


class StationLocationSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.StationLocation
        fields = '__all__'


class StationStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.StationState
        fields = '__all__'


class TemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.Temperature
        fields = '__all__'


class WaterTemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.WaterTemperature
        fields = '__all__'


class WindDirectionSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.WindDirection
        fields = '__all__'


class WindSpeedSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = models.WindSpeed
        fields = '__all__'
