// RainProvider.h

#ifndef RAIN_PROVIDER_H
#define RAIN_PROVIDER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "IDataProvider.h"
#include "Constants.h"

class RainProvider : public IDataProvider {
	public:
		RainProvider();

		bool setup();

		// Gets "rain" value from sensor. This value basically describes how water saturated the sensor is.
		float getRainValue();

		// Gets heuristic rain state from input value(s).
		const String getRainState(float* rainValue);
};

#endif
