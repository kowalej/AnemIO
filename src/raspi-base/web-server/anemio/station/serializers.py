from rest_framework import serializers
from .models import *
from .serializerfields import UnixDateTimeField
import pytz

timestamp_serializer = UnixDateTimeField(assume_milliseconds=True)

class AccelerometerXyzSerializer(serializers.ModelSerializer):
    timestamp = timestamp_serializer
    timestamp_dt = serializers.DateTimeField(source='timestamp')

    class Meta:
        model = AccelerometerXyz
        fields = '__all__'
