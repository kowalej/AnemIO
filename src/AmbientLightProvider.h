// AmbientLightProvider.h

#ifndef _AMBIENT_LIGHT_PROVIDER_h
#define _AMBIENT_LIGHT_PROVIDER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IDataProvider.h"
#include "Constants.h"

class AmbientLightProvider : public IDataProvider {
public:
	AmbientLightProvider();

	bool setup();

	// Gets ambient light value from sensor.
	// When it’s in an extremely dark room, it reads zero on the Arduino’s 10 bit A/D.
	// When it’s taken outside into direct sunlight it reads around 960 (which is not even saturated yet – maybe in the tropics it would hit 1023.
	float getAmbientLightValue();

	// Gets heuristic ambient light state from input value(s).
	const String getAmbientLightState(const float &lightValues);
};


#endif

