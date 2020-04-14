# Create your tasks here
from __future__ import absolute_import, unicode_literals

from celery import shared_task
from station import constants, models
from datetime import timezone, datetime
import logging


# Create your views here.
logger = logging.getLogger(__name__)


@shared_task
def station_request_sleep():
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.SLEEP_REQUESTED.value
        )
    except Exception:
        logger.error('Exception during request station sleep.')
        raise Exception('Sleep task incomplete: Exception during request station sleep.')


@shared_task
def station_request_wake():
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.WAKE_REQUESTED.value
        )
    except Exception:
        logger.error('Exception during request station wake.')
        raise Exception('Wake task incomplete: Exception during request station wake.')


@shared_task
def station_request_calibrate():
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.CALIBRATE_REQUESTED.value
        )
    except Exception:
        logger.error('Exception during request station calibrate.')
        raise Exception('Calibration task incomplete: Exception during request station calibration.')


@shared_task
def station_request_offline():
    try:
        models.StationState.objects.create(
            timestamp=datetime.datetime.utcnow(),
            state=constants.StationState.OFFLINE.value
        )
    except Exception:
        logger.error('Exception during set station offline.')
        raise Exception('Offline task incomplete: Exception during set station offline.')


@shared_task
def push_data(negative_offset_seconds, start_datetime=datetime.now(timezone.utc), destination='pwsweather'):
    if destination == 'pwsweather':
        pass
    else:
        raise Exception('Unsupported weather report destination: {0}.'.format(destination))
