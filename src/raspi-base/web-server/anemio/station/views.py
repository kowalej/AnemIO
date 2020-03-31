from rest_framework import status
from rest_framework.decorators import api_view, action, permission_classes
from rest_framework import viewsets
from rest_framework.response import Response
from rest_framework import permissions
from rest_framework.filters import OrderingFilter
import station.models as models
import station.serializers as serializers
import station.filters as filters
from django_filters.rest_framework.backends import DjangoFilterBackend
import datetime
from station import constants
import logging
import django.utils as utils
import pytz


# Create your views here.
f = permissions.DjangoModelPermissions
logger = logging.getLogger(__name__)


class AccelerometerXyzViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.AccelerometerXyz.objects.all()
    serializer_class = serializers.AccelerometerXyzSerializer
    filterset_class = filters.AccelerometerXyzFilterSet


class AmbientLightStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.AmbientLightState.objects.all()
    serializer_class = serializers.AmbientLightStateSerializer
    filterset_class = filters.AmbientLightStateFilterSet


class AmbientLightValuesViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.AmbientLightValues.objects.all()
    serializer_class = serializers.AmbientLightValuesSerializer
    filterset_class = filters.AmbientLightValuesFilterSet


class BatteryLevel(viewsets.ReadOnlyModelViewSet):
    queryset = models.BatteryLevel.objects.all()
    serializer_class = serializers.BatteryLevelSerializer
    filterset_class = filters.BatteryLevelFilterSet


class CompassHeadingViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.CompassHeading.objects.all()
    serializer_class = serializers.CompassHeadingSerializer
    filterset_class = filters.CompassHeadingFilterSet


class CompassXyzViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.CompassXyz.objects.all()
    serializer_class = serializers.CompassXyzSerializer
    filterset_class = filters.CompassXyzFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class DeviceStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.DeviceState.objects.all()
    serializer_class = serializers.DeviceStateSerializer
    filterset_class = filters.DeviceStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class HumidityViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.Humidity.objects.all()
    serializer_class = serializers.HumiditySerializer
    filterset_class = filters.HumidityFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressureAltitudeViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.PressureAltitude.objects.all()
    serializer_class = serializers.PressureAltitudeSerializer
    filterset_class = filters.PressureAltitudeFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressurePressureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.PressurePressure.objects.all()
    serializer_class = serializers.PressurePressureSerializer
    filterset_class = filters.PressurePressureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class PressureTemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.PressureTemperature.objects.all()
    serializer_class = serializers.PressureTemperatureSerializer
    filterset_class = filters.PressureTemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class RainStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.RainState.objects.all()
    serializer_class = serializers.RainStateSerializer
    filterset_class = filters.RainStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class RainValuesViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.RainValues.objects.all()
    serializer_class = serializers.RainValuesSerializer
    filterset_class = filters.RainValuesFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class StationLocationViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.StationLocation.objects.all()
    serializer_class = serializers.StationLocationSerializer
    filterset_class = filters.StationLocationFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class StationStateViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.StationState.objects.all()
    serializer_class = serializers.StationStateSerializer
    filterset_class = filters.StationStateFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class TemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.Temperature.objects.all()
    serializer_class = serializers.TemperatureSerializer
    filterset_class = filters.TemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WaterTemperatureViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.WaterTemperature.objects.all()
    serializer_class = serializers.WaterTemperatureSerializer
    filterset_class = filters.WaterTemperatureFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WindDirectionViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.WindDirection.objects.all()
    serializer_class = serializers.WindDirectionSerializer
    filterset_class = filters.WindDirectionFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


class WindSpeedViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = models.WindSpeed.objects.all()
    serializer_class = serializers.WindSpeedSerializer
    filterset_class = filters.WindSpeedFilterSet
    filter_backends = [OrderingFilter, DjangoFilterBackend]
    ordering_fields = ['timestamp']
    ordering = ['-timestamp']


@permission_classes((permissions.IsAdminUser,))
class StationHealthViewSet(viewsets.ViewSet):
    def list(self, request, format=None):
        try:
            state = models.StationState.objects.latest('timestamp').state
            device_status = models.DeviceState.objects.latest('timestamp')
            battery_level = models.BatteryLevel.objects.latest('timestamp')

            # Get uptime.
            cold_boot = models.StationState.objects.filter(
                state=constants.StationState.SETUP_BOOT.value
            ).latest('timestamp')
            now = utils.timezone.make_aware(datetime.datetime.utcnow(), timezone=pytz.utc)
            if cold_boot is not None:
                uptime = now - cold_boot.timestamp
            else:
                uptime = datetime.timedelta(0, 0, 0)

            # Check what devices went offline since boot.
            any_offline_since_boot = models.DeviceState.objects.filter(timestamp__gt=cold_boot.timestamp).exclude(offline_devices='[]')
            any_offline_since_boot_devices = []
            for device in any_offline_since_boot:
                offline_devices = [int(x) for x in device.offline_devices.replace('[', '').replace(']', '').split(',') if len(x) > 0]
                any_offline_since_boot_devices.extend(offline_devices)
            any_offline_since_boot_devices = set(any_offline_since_boot_devices)
        except Exception as e:
            print(e)
            logger.error('Exception during health check.')
            return Response(
                {
                    'success': False,
                    'error': '''Could not perform health check, an error was encountered when processing values.'''
                },
                status=status.HTTP_500_INTERNAL_SERVER_ERROR
            )
        online_devices = [int(x) for x in device_status.online_devices.replace('[', '').replace(']', '').split(',') if len(x) > 0]
        offline_devices = [int(x) for x in device_status.offline_devices.replace('[', '').replace(']', '').split(',') if len(x) > 0]
        return Response(
            {
                'state': str(constants.StationState(state)),
                'uptime': str(uptime),
                'booted_at': cold_boot.timestamp,
                'battery_percent': battery_percent,
                'online_devices': [str(constants.Devices(x)) for x in online_devices],
                'offline_devices': [str(constants.Devices(x)) for x in offline_devices],
                'went_offline_since_boot_devices': [str(constants.Devices(x)) for x in any_offline_since_boot_devices],
                'state_raw': state,
                'online_devices_raw': online_devices,
                'offline_devices_raw': offline_devices,
                'went_offline_since_boot_devices_raw': any_offline_since_boot_devices
            },
            status=status.HTTP_200_OK
        )


@permission_classes((permissions.IsAdminUser,))
class StationControlRestartViewSet(viewsets.ViewSet):
    def list(self, request, format=None):
        try:
            models.StationState.objects.create(
                timestamp=datetime.datetime.utcnow(),
                state=constants.StationState.RESTART_REQUESTED.value
            )
        except Exception:
            logger.error('Exception during request station restart.')
            return Response(
                {
                    'success': False,
                    'error': '''Could not request station restart, an error was encountered when writing RESTART_REQUESTED state.'''
                },
                status=status.HTTP_500_INTERNAL_SERVER_ERROR
            )
        return Response(
            {
                'success': True,
                'message': ' Station will be restarted within ~30 seconds.'
            },
            status=status.HTTP_200_OK
        )


@permission_classes((permissions.IsAdminUser,))
class StationControlSleepViewSet(viewsets.ViewSet):
    def list(self, request, format=None):
        try:
            models.StationState.objects.create(
                timestamp=datetime.datetime.utcnow(),
                state=constants.StationState.SLEEP_REQUESTED.value
            )
        except Exception:
            logger.error('Exception during request station sleep.')
            return Response(
                {
                    'success': False,
                    'error': '''Could not request station sleep, an error was encountered when writing SLEEP_REQUESTED state.'''
                },
                status=status.HTTP_500_INTERNAL_SERVER_ERROR
            )
        return Response(
            {
                'success': True,
                'message': ' Station will go to sleep within ~30 seconds.'
            },
            status=status.HTTP_200_OK
        )


@permission_classes((permissions.IsAdminUser,))
class StationControlWakeViewSet(viewsets.ViewSet):
    def list(self, request, format=None):
        try:
            models.StationState.objects.create(
                timestamp=datetime.datetime.utcnow(),
                state=constants.StationState.WAKE_REQUESTED.value
            )
        except Exception:
            logger.error('Exception during request station wake.')
            return Response(
                {
                    'success': False,
                    'error': '''Could not request station wake, an error was encountered when writing WAKE_REQUESTED state.'''
                },
                status=status.HTTP_500_INTERNAL_SERVER_ERROR
            )
        return Response(
            {
                'success': True,
                'message': ' Station will be awoken within ~30 seconds.'
            },
            status=status.HTTP_200_OK
        )
