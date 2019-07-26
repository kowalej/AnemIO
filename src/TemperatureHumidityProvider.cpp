#include "TemperatureHumidityProvider.h"

TemperatureHumidityProvider::TemperatureHumidityProvider() {}

bool TemperatureHumidityProvider::setup()
{
	_isOnline = false;
	return _isOnline;
}


