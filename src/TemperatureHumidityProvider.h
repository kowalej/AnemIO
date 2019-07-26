// TemperatureHumidityProvider.h

#ifndef TEMPERATURE_HUMIDITY_PROVIDER_h
#define TEMPERATURE_HUMIDITY_PROVIDER_h

#include "IDataProvider.h"

class TemperatureHumidityProvider : public IDataProvider
{
	public:
		TemperatureHumidityProvider();
		bool setup();
};

#endif

