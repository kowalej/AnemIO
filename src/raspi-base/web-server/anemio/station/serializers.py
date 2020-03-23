from rest_framework import serializers
from .models import *
from .serializerfields import UnixDateTimeField
import pytz

timestamp_serializer = UnixDateTimeField(assume_milliseconds=True)
timestamp_dt_serialize = serializers.DateTimeField(source='timestamp')

class AccelerometerXyzSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = AccelerometerXyz
        fields = '__all__'

class AmbientLightStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = AmbientLightState
        fields = '__all__'


class AmbientLightValuesSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = AmbientLightValues
        fields = '__all__'


class CompassHeadingSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = CompassHeading
        fields = '__all__'


class CompassXyzSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = CompassXyz
        fields = '__all__'


class DeviceStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize
    # online_devices = serializers.SerializerMethodField()

    # def get_online_devices(self, obj):
    #     online_devices = obj.online_devices
    #     if online_devices is not None:
    #         devices_typed = []
    #         online_devices = online_devices.replace('[','').replace(']','').split(',')
    #         for device in online_devices:
    #             devices_typed.append(str(constants.Devices(int(device))))
    #         return devices_typed
    #     return online_devices

    class Meta:
        model = DeviceState
        fields = '__all__'


class HumiditySerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = Humidity
        fields = '__all__'


class PressureAltitudeSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = PressureAltitude
        fields = '__all__'


class PressurePressureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = PressurePressure
        fields = '__all__'


class PressureTemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = PressureTemperature
        fields = '__all__'


class RainStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = RainState
        fields = '__all__'


class RainValuesSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = RainValues
        fields = '__all__'


class StationLocationSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = StationLocation
        fields = '__all__'


class StationStateSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = StationState
        fields = '__all__'


class TemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = Temperature
        fields = '__all__'


class WaterTemperatureSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = WaterTemperature
        fields = '__all__'


class WindDirectionSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = WindDirection
        fields = '__all__'


class WindSpeedSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = timestamp_dt_serialize

    class Meta:
        model = WindSpeed
        fields = '__all__'
