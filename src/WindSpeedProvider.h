// WindProvider.h

#ifndef WIND_SPEED_PROVIDER_H
#define WIND_SPEED_PROVIDER_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IDataProvider.h"
#include "Constants.h"
#include <SerialDebug.h>

class WindSpeedProvider : public IDataProvider {
	public:
		WindSpeedProvider();
		bool setup();

		// Gets "raw" wind speed (no temperature correction), in knots.
		float getWindSpeedRaw();

		// Gets wind sensor temperature, in °C.
		float getWindSensorTemperature();

		// Gets a temperature corrected wind speed. Temperature in degrees °C.
		float getCorrectedWindSpeed(float temperature);
};

#endif

