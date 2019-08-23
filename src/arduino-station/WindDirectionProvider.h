// WindDirectionProvider.h

#ifndef WIND_DIRECTION_PROVIDER_H
#define WIND_DIRECTION_PROVIDER_H

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

		// Get heading based on magnet position. This is relative to the station, not factoring in compass heading.
		int getHeading();
		// This gets the heading corrected for the station's compass heading.
		int getCorrectedHeading(int compassHeading);
	
	private:
		AS5040 enc;
		int _numConsecutiveFaults = 0;
		void checkFaults();
};

#endif

