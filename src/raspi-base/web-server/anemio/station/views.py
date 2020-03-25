from rest_framework import status
from rest_framework.decorators import api_view, permission_classes
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


# Create your views here.
f = permissions.DjangoModelPermissions


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


@api_view(['POST'])
@permission_classes((permissions.AllowAny,))
def station_restart(request):
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.RESTART_REQUESTED.value
        )
    except Exception as e:
        print(e)
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


@api_view(['GET'])
def station_sleep(request):
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.SLEEP_REQUESTED
        )
    except Exception:
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


@api_view(['GET'])
def station_wake(request):
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.WAKE_REQUESTED
        )
    except Exception:
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
