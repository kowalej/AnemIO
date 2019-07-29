// AnemioStation.h

#ifndef ANEMIO_STATION_H
#define ANEMIO_STATION_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include "PressureProvider.h"
#include "RainProvider.h"
#include "TemperatureHumidityProvider.h"
#include "Constants.h"
#include "Pair.h"
#include "SampleSet.h"
#include <SoftReset.h>
#include <LowPower.h>
#include <SerialDebug.h>

class AnemioStation {
    public:
        AnemioStation();
        void setup();
	    void loop();
		
		// Sets up devices and reports number that are offline (i.e. couldn't be setup).
		int setupProviders(int numberOfRetries = 3);

		// Checks health of devices and reports number that are offline.
		int healthCheck();

	private:
		SampleSet _sampleSet; // Stores and serializes the sampling infromation from all sensors.

		PressureProvider _pressureProvider;
		RainProvider _rainProvider;
		TemperatureHumidityProvider _temperatureHumidityProvider;

		bool _online[Devices::TOTAL];
		unsigned long _lastCheck[Devices::TOTAL];
};

#endif