#include "WaterTemperatureProvider.h"

WaterTemperatureProvider::WaterTemperatureProvider() {}

bool WaterTemperatureProvider::setup() {
	pinMode(WATER_TEMP_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float WaterTemperatureProvider::getWaterTemperature() {
	float average = 0;
	for (int i = 0; i < WATER_TEMP_ANALOG_READ_SAMPLE_COUNT; i++) {
		average += analogRead(WATER_TEMP_SENSOR_INPUT_PIN);
	}
	average = average / WATER_TEMP_ANALOG_READ_SAMPLE_COUNT;

	// Info regarding calculations below is available here: https://learn.adafruit.com/thermistor/using-a-thermistor.
	average = WATER_TEMP_EXTERNAL_RESISTOR_OHMS * average / (1023 - average);

	float steinhart;
	steinhart = average / WATER_TEMP_THERMISTOR_NOMINAL_OHMS;					// (R/Ro)
	steinhart = log(steinhart);													// ln(R/Ro)
	steinhart /= WATER_TEMP_THERMISTOR_BETA_COEFFICIENT;						// 1/B * ln(R/Ro)
	steinhart += 1.0f / (WATER_TEMP_THERMISTOR_NOMINAL_TEMPERATURE + 273.15f);	// + (1/To)
	steinhart = 1.0f / steinhart;												// Invert
	steinhart -= 273.15f;														// Convert to Celcius.

	return steinhart;
}



