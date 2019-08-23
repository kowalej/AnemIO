// WaterTemperatureProvider.h

#ifndef WATER_TEMPERATURE_PROVIDER_H
#define WATER_TEMPERATURE_PROVIDER_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Constants.h"
#include "IDataProvider.h"

class WaterTemperatureProvider : public IDataProvider {
	public:
		WaterTemperatureProvider();

		bool setup();

		// Gets water temperature in celcius.
		float getWaterTemperature();
};

#endif

