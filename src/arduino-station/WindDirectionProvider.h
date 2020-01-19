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
#include <EEPROMVar.h>
#include <EEPROMex.h>

class WindDirectionProvider : public IDataProvider {
	public:
		WindDirectionProvider();
		bool setup();

		// Get heading based on magnet position. This is relative to the station, not factoring in compass heading.
		int getHeading();
		// This gets the heading corrected for the station's compass heading.
		int getCorrectedHeading(int windHeading, int compassHeading);
		// Calibrates where the zero position is. This should be where the tip of the wind sock points in line with the compass X axis.
		void calibrateZero();
	
	private:
		AS5040 _enc;
		int _numConsecutiveFaults = 0;
		int _zeroPosition;
		void checkFaults();
};

#endif

