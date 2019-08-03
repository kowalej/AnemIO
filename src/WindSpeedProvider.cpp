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
	return pow((((float)sampledAvg - 264.0) / 85.6814), 3.36814) * 0.8689758250; // The last multiplication converts from MPH to knots
}

float WindSpeedProvider::getWindTemperature()
{
	float total = 0;
	for (int i = 0; i < WIND_SPEED_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(WIND_SPEED_SENSOR_TEMPERATURE_INPUT_PIN);
	}
	float smpledAvg = total / WIND_SPEED_ANALOG_READ_SAMPLE_COUNT;

	// From Modern Device:
	// Convert to volts then use formula from datatsheet
	// Vout = ( TempC * .0195 ) + .400
	// tempC = (Vout - V0c) / TC   see the MCP9701 datasheet for V0c and TC
	return ((((float)smpledAvg * 5.0) / 1024.0) - 0.400) / .0195;
}

float WindSpeedProvider::getCorrectedWindSpeed(float speed, float temperature) {
	return NAN;
}