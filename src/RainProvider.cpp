#include "RainProvider.h"

RainProvider::RainProvider() { }

bool RainProvider::setup()
{
	pinMode(RAIN_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float RainProvider::getRainValue()
{
	float total = 0;
	for (int i = 0; i < RAIN_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(RAIN_SENSOR_INPUT_PIN);
	}
	return total / RAIN_ANALOG_READ_SAMPLE_COUNT;
}

const String RainProvider::getRainState(float* rainValue)
{
	return "fuckoff";
}
