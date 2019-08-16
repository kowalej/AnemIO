#include "WindSpeedProvider.h"

WindSpeedProvider::WindSpeedProvider() { }

bool WindSpeedProvider::setup()
{
	pinMode(WIND_SPEED_SENSOR_SPEED_INPUT_PIN, INPUT);
	pinMode(WIND_SPEED_SENSOR_TEMPERATURE_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float WindSpeedProvider::getWindSpeedRaw()
{
	float total = 0;
	for (int i = 0; i < WIND_SPEED_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(WIND_SPEED_SENSOR_SPEED_INPUT_PIN);
	}
	float sampledAvg = total / WIND_SPEED_ANALOG_READ_SAMPLE_COUNT;

	// From Modern Device:
	// Wind formula derived from a wind tunnel data, annemometer and some fancy Excel regressions.
	return pow((((float)sampledAvg - 264.0f) / 85.6814f), 3.36814f) * 0.8689758250f; // The last multiplication converts from MPH to knots.
}

float WindSpeedProvider::getWindSensorTemperature()
{
	float total = 0;
	for (int i = 0; i < WIND_SPEED_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(WIND_SPEED_SENSOR_TEMPERATURE_INPUT_PIN);
	}
	float sampledAvg = total / WIND_SPEED_ANALOG_READ_SAMPLE_COUNT;

	// From Modern Device:
	// Convert to volts then use formula from datatsheet
	// Vout = ( TempC * .0195 ) + .400
	// tempC = (Vout - V0c) / TC   see the MCP9701 datasheet for V0c and TC
	float voltage = sampledAvg * (5.0f / 1023.0f);
	return (voltage - 0.400f) / 0.0195f;
}

float WindSpeedProvider::getCorrectedWindSpeed(float temperature) 
{
	float total = 0;
	for (int i = 0; i < WIND_SPEED_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(WIND_SPEED_SENSOR_SPEED_INPUT_PIN);
	}
	float sampledAvg = total / WIND_SPEED_ANALOG_READ_SAMPLE_COUNT;

	// Get our voltage value back from the ADC value.
	float voltage = sampledAvg * (5.0 / 1023.0);

	// See https://moderndevice.com/news/calibrating-rev-p-wind-sensor-new-regression/#more-19365.
	float windSpeed = pow((((voltage - WIND_SPEED_SENSOR_ZERO_WIND_VOLTAGE) / (3.038517 * pow(temperature, 0.115157))) / 0.087288), 3.009364);
	return windSpeed;
}