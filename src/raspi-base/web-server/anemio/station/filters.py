import django_filters
from .models import *

timestamp_dt_filter = django_filters.IsoDateTimeFromToRangeFilter(label='Timestamp Date and Time (ISO8601)',)

class AccelerometerXyzFilter(django_filters.FilterSet):
    timestamp = timestamp_dt_filter

    class Meta:
        model = AccelerometerXyz
        fields = ['timestamp']
