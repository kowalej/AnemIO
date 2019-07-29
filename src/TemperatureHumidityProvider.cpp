#include "TemperatureHumidityProvider.h"

TemperatureHumidityProvider::TemperatureHumidityProvider() : 
	_temperatureHumditySensor(DHT(TEMPERATURE_HUMIDITY_INPUT_PIN, DHT22)) {
}

bool TemperatureHumidityProvider::setup()
{
	_temperatureHumditySensor.begin();
	_isOnline = true;
	return _isOnline;
}

void TemperatureHumidityProvider::checkfaults() {
	if (_numConsecutiveFaultsTemperature > TEMPERATURE_HUMIDITY_FAULT_MAX
		|| _numConsecutiveFaultsHumidity > TEMPERATURE_HUMIDITY_FAULT_MAX) {
		_isOnline = false;
	}
}

float TemperatureHumidityProvider::getTemperature() {
	checkfaults();

	// Read temperature as Celsius (the default).
	float t = _temperatureHumditySensor.readTemperature();

	// Check if any reads failed and exit early (to try again).
	if(isnan(t)) {
		printW("Failed to read temperature from DHT-22 temperature / humidity sensor!");
		_numConsecutiveFaultsTemperature += 1;
		return;
	}

	// Good read, reset faults and return value;
	_numConsecutiveFaultsTemperature = 0;
	return t;
}

float TemperatureHumidityProvider::getHumidity() {
	checkfaults();

	// Read humidity as %.
	float h = _temperatureHumditySensor.readHumidity();

	// Check if any reads failed and exit early (to try again).
	if (isnan(h)) {
		printW("Failed to read humidity from DHT-22 temperature / humidity sensor!");
		_numConsecutiveFaultsHumidity += 1;
		return;
	}

	// Good read, reset faults and return value;
	_numConsecutiveFaultsHumidity = 0;
	return h;
}