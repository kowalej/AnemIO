#include "WindDirectionProvider.h"

WindDirectionProvider::WindDirectionProvider() : _enc(WIND_DIRECTION_SENSOR_CLK_PIN, WIND_DIRECTION_SENSOR_CS_PIN, WIND_DIRECTION_SENSOR_DO_PIN) {}

bool WindDirectionProvider::setup() {
	if (!_enc.begin()) {
		debugE("Setup failed, cannot find valid AS5040 magnetometer. Check wiring and your AS5040.");
		return false;
	}
	
	// Get the zero position that is stored in EEPROM.
	_zeroPosition = EEPROM.readInt(EEPROM_WIND_DIRECTION_ZERO);
	if (_zeroPosition == -1) {
		_zeroPosition = 0; // Never calibrated, default to zero.
	}

	debugI("Raw zero position of wind direction sensor (AS5040) is %d.", _zeroPosition);
	return true;
}

int compare(const void* a, const void* b)
{
	const int* x = (int*)a;
	const int* y = (int*)b;
	if (*x > * y)
		return 1;
	else if (*x < *y)
		return -1;
	return 0;
}

void WindDirectionProvider::calibrateZero() {
	debugI("Calibration the raw zero position of wind direction sensor (AS5040), sampling position for %d seconds...", WIND_DIRECTION_ZERO_TIME_SECONDS);
	int delayTimeMillis = WIND_DIRECTION_ZERO_TIME_SECONDS * 1000;
	int values[WIND_DIRECTION_ZERO_NUM_SAMPLES];
	for(int i = 0; i < WIND_DIRECTION_ZERO_NUM_SAMPLES; i++) {
		unsigned long timestart = millis();
		values[i] = _enc.read();
		unsigned long timeend = millis();
		delay(delayTimeMillis - (timeend - timestart));
	}

	// Sort the values so we can do a median average.
	qsort(values, WIND_DIRECTION_ZERO_NUM_SAMPLES, sizeof(int), compare);
	
	int start, end;
	// Even number of samples (take middle 4 values).
	if (WIND_DIRECTION_ZERO_NUM_SAMPLES % 2 == 0) {
		int mid = WIND_DIRECTION_ZERO_NUM_SAMPLES / 2;
		start = mid - 2;
		end = mid + 1;
	}
	// Odd number of samples (take middle 3 values).
	else {
		int mid = WIND_DIRECTION_ZERO_NUM_SAMPLES / 2;
		start = mid - 1;
		end = mid + 1;
	}
	int average = 0;
	for (int i = start; i <= end; i++) {
		average += values[i];
	}
	average = average / (float)(end - start + 1);
	_zeroPosition = average;
	
	// Save value so we have it on next boot.
	EEPROM.writeInt(EEPROM_WIND_DIRECTION_ZERO, _zeroPosition);

	debugI("Done calibration. New zero position is %d.", _zeroPosition);
}

void WindDirectionProvider::checkFaults() {
	if (_numConsecutiveFaults > WIND_DIRECTION_FAULT_MAX) {
		_isOnline = false;
	}
	if (_enc.status() == 19) {
		debugE("AS5040 status indicates that the magenetic field is out of range.");
	}
}

int WindDirectionProvider::getHeading() {
	checkFaults();
	int value = _enc.read();

	if (_enc.valid() == false) {
		_numConsecutiveFaults += 1;
		return -1;
	}
	else {
		_numConsecutiveFaults = 0;
	}

	// Convert to 360 heading.
	value = (value - _zeroPosition) % 1024;
	int normalized = (1024.0f / value) * 360; // Normalize the reading.
	return (normalized + 180) % 360; // "Flip" the direction - since the wind will blow from the opposite direction of the position.
}

int WindDirectionProvider::getCorrectedHeading(int windHeading, int compassHeading)
{
	return ((windHeading + compassHeading) % 360); // Get the "absolute" direction based on platform position.
}


