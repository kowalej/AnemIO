// WindDirectionProvider.h

#ifndef WIND_DIRECTION_PROVIDER_h
#define WIND_DIRECTION_PROVIDER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IDataProvider.h"
#include <AS5040.h>
#include "Constants.h"
#include "SerialDebug.h"

class WindDirectionProvider : public IDataProvider {
	public:
		WindDirectionProvider();
		bool setup();

		int getHeading();
		int getCorrectedHeading(int compassHeading);
	
	private:
		AS5040 enc;
		int _numConsecutiveFaults = 0;
		void checkFaults();
};

#endif

