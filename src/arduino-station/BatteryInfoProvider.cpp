#include "BatteryInfoProvider.h"

BatteryInfoProvider::BatteryInfoProvider() { }

bool BatteryInfoProvider::setup()
{
	pinMode(WATER_TEMP_SENSOR_INPUT_PIN, INPUT);

	// Note we will only power the thermistor when reading the temperature to avoid generating heat and wasting power.
	pinMode(WATER_TEMP_SENSOR_TRIGGER_PIN, OUTPUT);
	digitalWrite(WATER_TEMP_SENSOR_TRIGGER_PIN, LOW);
	_isOnline = true;
	return _isOnline;
}

void BatteryInfoProvider::checkFaults() {
	if (_numConsecutiveBatteryLevelFaults > BATTERY_LEVEL_FAULT_MAX) {
		_isOnline = false;
	}
}

float BatteryInfoProvider::getBatteryLevel() {
	checkFaults();

	// Read temperature as Celsius (the default).
	float t = // ANALOG READ

	// Check if any reads failed and exit early (to try again).
	if (// check if outside of 12v range) {
		printW("Failed to read battery level, voltage reading is out of 12V range!");
		_numConsecutiveBatteryLevelFaults += 1;
		return NAN;
	}

	// Good read, reset faults and return value.
	_numConsecutiveBatteryLevelFaults = 0;
	return t;
}