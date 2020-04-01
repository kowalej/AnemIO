import django_filters
from .models import *
from . import constants


timestamp_dt_filter = django_filters.IsoDateTimeFromToRangeFilter(label='Timestamp Date and Time (ISO8601)',)


class AccelerometerXyzFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    x = django_filters.RangeFilter()
    y = django_filters.RangeFilter()
    z = django_filters.RangeFilter()

    class Meta:
        model = AccelerometerXyz
        fields = ['timestamp', 'x', 'y', 'z']


class AmbientLightStateFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    state = django_filters.CharFilter(lookup_expr='in')

    class Meta:
        model = AmbientLightState
        fields = ['timestamp', 'state']


class AmbientLightValuesFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = AmbientLightValues
        fields = ['timestamp', 'value']


class BatteryLevelFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = BatteryLevel
        fields = ['timestamp', 'value']


class CompassHeadingFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    heading = django_filters.RangeFilter()

    class Meta:
        model = CompassHeading
        fields = ['timestamp', 'heading']


class CompassXyzFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    x = django_filters.RangeFilter()
    y = django_filters.RangeFilter()
    z = django_filters.RangeFilter()

    class Meta:
        model = CompassXyz
        fields = ['timestamp', 'x', 'y', 'z']


class DeviceStateFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    is_setup = django_filters.BooleanFilter()
    
    class Meta:
        model = DeviceState
        fields = ['timestamp', 'is_setup']


class HumidityFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = Humidity
        fields = ['timestamp', 'value']


class PressureAltitudeFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = PressureAltitude
        fields = ['timestamp', 'value']


class PressurePressureFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = PressurePressure
        fields = ['timestamp', 'value']


class PressureTemperatureFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = PressureTemperature
        fields = ['timestamp', 'value']


class RainStateFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    state = django_filters.CharFilter(lookup_expr='in')

    class Meta:
        model = RainState
        fields = ['timestamp', 'state']


class RainValuesFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = RainValues
        fields = ['timestamp', 'value']


class StationLocationFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    lat = django_filters.RangeFilter()
    lon = django_filters.RangeFilter()
    is_actual = django_filters.BooleanFilter()

    class Meta:
        model = StationLocation
        fields = ['timestamp', 'lat', 'lon', 'is_actual']


class StationStateFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    state = django_filters.TypedMultipleChoiceFilter(choices=[(tag.value, tag) for tag in constants.StationState])

    class Meta:
        model = StationState
        fields = ['timestamp']


class TemperatureFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = Temperature
        fields = ['timestamp', 'value']


class WaterTemperatureFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = WaterTemperature
        fields = ['timestamp', 'value']


class WindDirectionFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()

    class Meta:
        model = WindDirection
        fields = ['timestamp', 'value']


class WindSpeedFilterSet(django_filters.FilterSet):
    timestamp = timestamp_dt_filter
    value = django_filters.RangeFilter()
    temperature = django_filters.RangeFilter()

    class Meta:
        model = WindSpeed
        fields = ['timestamp', 'value', 'temperature']
