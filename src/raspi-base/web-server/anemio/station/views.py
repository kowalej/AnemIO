from rest_framework import status
from rest_framework.decorators import api_view
from rest_framework import viewsets
from rest_framework.response import Response
from rest_framework import permissions
from rest_framework.filters import OrderingFilter
from .models import *
from .serializers import *
from .filters import *
from django_filters.rest_framework.backends import DjangoFilterBackend

# Create your views here.
f = permissions.DjangoModelPermissions

class AccelerometerXyzViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = AccelerometerXyz.objects.all()
    serializer_class = AccelerometerXyzSerializer
    filterset_class = AccelerometerXyzFilterSet


class AmbientLightStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = AmbientLightState.objects.all()
    serializer_class = AmbientLightStateSerializer
    filterset_class = AmbientLightStateFilterSet


class AmbientLightValuesViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = AmbientLightValues.objects.all()
    serializer_class = AmbientLightValuesSerializer
    filterset_class = AmbientLightValuesFilterSet


class CompassHeadingViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = CompassHeading.objects.all()
    serializer_class = CompassHeadingSerializer
    filterset_class = CompassHeadingFilterSet


class CompassXyzViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = CompassXyz.objects.all()
    serializer_class = CompassXyzSerializer
    filterset_class = CompassXyzFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class DeviceStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = DeviceState.objects.all()
    serializer_class = DeviceStateSerializer
    filterset_class = DeviceStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class HumidityViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = Humidity.objects.all()
    serializer_class = HumiditySerializer
    filterset_class = HumidityFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressureAltitudeViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = PressureAltitude.objects.all()
    serializer_class = PressureAltitudeSerializer
    filterset_class = PressureAltitudeFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressurePressureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = PressurePressure.objects.all()
    serializer_class = PressurePressureSerializer
    filterset_class = PressurePressureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressureTemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = PressureTemperature.objects.all()
    serializer_class = PressureTemperatureSerializer
    filterset_class = PressureTemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class RainStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = RainState.objects.all()
    serializer_class = RainStateSerializer
    filterset_class = RainStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class RainValuesViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = RainValues.objects.all()
    serializer_class = RainValuesSerializer
    filterset_class = RainValuesFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class StationLocationViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = StationLocation.objects.all()
    serializer_class = StationLocationSerializer
    filterset_class = StationLocationFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class StationStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = StationState.objects.all()
    serializer_class = StationStateSerializer
    filterset_class = StationStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class TemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = Temperature.objects.all()
    serializer_class = TemperatureSerializer
    filterset_class = TemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WaterTemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = WaterTemperature.objects.all()
    serializer_class = WaterTemperatureSerializer
    filterset_class = WaterTemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WindDirectionViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = WindDirection.objects.all()
    serializer_class = WindDirectionSerializer
    filterset_class = WindDirectionFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WindSpeedViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = WindSpeed.objects.all()
    serializer_class = WindSpeedSerializer
    filterset_class = WindSpeedFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']

