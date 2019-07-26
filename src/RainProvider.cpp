#include "RainProvider.h"

RainProvider::RainProvider() { }

bool RainProvider::setup()
{
	pinMode(RAIN_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
}

float RainProvider::getRainValue()
{
	float total = 0;
	for (int i = 0; i < 300; i++) {
		total += analogRead(RAIN_SENSOR_INPUT_PIN);
	}
	return total / 300;
}

const char* RainProvider::getRainState(float rainValue)
{

	if (rainValue > 0) return "fuckyou";
	return "fuck you fo sho";
}
