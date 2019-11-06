#include "WindDirectionProvider.h"

WindDirectionProvider::WindDirectionProvider() : enc(WIND_DIRECTION_SENSOR_CLK_PIN, WIND_DIRECTION_SENSOR_CS_PIN, WIND_DIRECTION_SENSOR_DO_PIN) {}

bool WindDirectionProvider::setup() {
	_isOnline = true;
	if (!enc.begin()) {
		_isOnline = false;
		debugE("Setup failed, cannot find valid AS5040 magnetometer. Check wiring and your AS5040.");
	}
	return _isOnline;
}

void WindDirectionProvider::checkFaults() {
	if (_numConsecutiveFaults > WIND_DIRECTION_FAULT_MAX) {
		_isOnline = false;
	}
	if (enc.status() == 19) {
		debugE("AS5040 status indicates that the magenetic field is out of range.");
	}
}

int WindDirectionProvider::getHeading() {
	checkFaults();
	int value = enc.read();

	if (enc.valid() == false) {
		_numConsecutiveFaults += 1;
	}
	else {
		_numConsecutiveFaults = 0;
	}

	// Convert to 360 heading.
	float val = (1024.0f / value) * 360; // Normalize the reading.
	return (val + 180 % 360); // "Flip" the direction - since the wind will blow from the opposite of the position.
}

int WindDirectionProvider::getCorrectedHeading(int windHeading, int compassHeading)
{
	return ((windHeading + compassHeading) % 360); // Get the "absolute" direction based on platform position.
}


