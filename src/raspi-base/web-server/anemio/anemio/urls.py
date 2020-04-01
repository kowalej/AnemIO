"""anemio URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/3.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from rest_framework import routers
from django.conf.urls import include, url
from station import views

# Routers provide an easy way of automatically determining the URL conf.
router = routers.DefaultRouter()

router.get_api_root_view().cls.__name__ = "Station Documentation"
router.get_api_root_view().cls.__doc__ = "The following are the various APIs available on the station."

# Health check and control commands.
router.register(r'station/health', views.StationHealthViewSet, basename='station/health')
router.register(r'station/control/calibrate', views.StationControlCalibrateViewSet, basename='station/control/calibrate')
router.register(r'station/control/restart', views.StationControlRestartViewSet, basename='station/control/restart')
router.register(r'station/control/sleep', views.StationControlSleepViewSet, basename='station/control/sleep')
router.register(r'station/control/wake', views.StationControlWakeViewSet, basename='station/control/wake')

# Raw station / sensor info.
router.register(r'station/state', views.StationStateViewSet)
router.register(r'station/location', views.StationLocationViewSet)
router.register(r'devices/state', views.DeviceStateViewSet)

# Raw data.
router.register(r'telemetry/accelerometer/xyz', views.AccelerometerXyzViewSet)
router.register(r'telemetry/ambientlight/state', views.AmbientLightStateViewSet)
router.register(r'telemetry/ambientlight/values', views.AmbientLightValuesViewSet)
router.register(r'telemetry/compass/heading', views.CompassHeadingViewSet)
router.register(r'telemetry/compass/xyz', views.CompassXyzViewSet)
router.register(r'telemetry/humidty', views.HumidityViewSet)
router.register(r'telemetry/pressure/altitude', views.PressureAltitudeViewSet)
router.register(r'telemetry/pressure/values', views.PressurePressureViewSet)
router.register(r'telemetry/pressure/temperature', views.PressureTemperatureViewSet)
router.register(r'telemetry/rain/state', views.RainStateViewSet)
router.register(r'telemetry/rain/values', views.RainValuesViewSet)
router.register(r'telemetry/temperature', views.TemperatureViewSet)
router.register(r'telemetry/watertemperature', views.WaterTemperatureViewSet)
router.register(r'telemetry/wind/direction', views.WindDirectionViewSet)
router.register(r'telemetry/wind/speed', views.WindSpeedViewSet)


urlpatterns = [
    path('admin/', admin.site.urls),
    url(r'^', include(router.urls)),
    url(r'^api-auth/', include('rest_framework.urls'))
]
