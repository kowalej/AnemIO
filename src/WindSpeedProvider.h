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

class WindSpeedProvider : public IDataProvider {
	public:
		WindSpeedProvider();
		bool setup();

		// Gets "raw" wind speed (no temperature correction).
		float getWindSpeedRaw();

		// Gets wind sensor temperature.
		float getWindTemperature();

		// Gets a temperature corrected wind speed.
		float getCorrectedWindSpeed(float speed, float temp);
};

#endif

