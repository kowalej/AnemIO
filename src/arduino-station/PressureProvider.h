// PressureProvider.h

#ifndef PRESSURE_PROVIDER_H
#define PRESSURE_PROVIDER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <BMx280MI.h>
#include "Constants.h"
#include "IDataProvider.h"
#include <SerialDebug.h>

class PressureProvider : public IDataProvider {
    public:
		PressureProvider();

        bool setup();

		// Gets the pressure in Pa (Pascals).
        float getPressure();

		// Gets the temperature in Celcius.
        float getTemperature();

		// Gets the calculated altitude (in meters) based on pressure.
        float getAltitude();

    private:
		BMx280I2C bmx280; // I2C connection.
		void measure();

		// Flags for avoiding multiple measurements.
		bool _temperatureRead = true;
		bool _pressureRead = true;
		bool _altitudeRead = true;
		float _lastPressure = NAN; // Keep track of last pressure value (to use for altitude calculation).
};

#endif