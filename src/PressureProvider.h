#ifndef PressureProvider_H
#define PressureProvider_H
#include <BMx280MI.h>
#include "Constants.h"

class PressureProvider {
    public:
		PressureProvider();

        void setup();
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
		bool temperatureRead = true;
		bool pressureRead = true;
		bool altitudeRead = true;
		float lastPressure = NAN; // Keep track of last pressure value (to use for altitude calculation).
};

#endif

