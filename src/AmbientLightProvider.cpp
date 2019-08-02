#include "AmbientLightProvider.h"

AmbientLightProvider::AmbientLightProvider() { }

bool AmbientLightProvider::setup()
{
	pinMode(AMBIENT_LIGHT_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float AmbientLightProvider::getAmbientLightValue()
{
	float total = 0;
	for (int i = 0; i < AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(AMBIENT_LIGHT_SENSOR_INPUT_PIN);
	}
	return total / AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT;
}

const String AmbientLightProvider::getAmbientLightState(const float& lightValues)
{
	if (lightValues > 0) return "fuckyou";
	return "fuck you fo sho";
}
