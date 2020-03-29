// AnemioStation.h

#ifndef ANEMIO_STATION_H
#define ANEMIO_STATION_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include "AmbientLightProvider.h"
#include "CompassAccelerometerProvider.h"
#include "PressureProvider.h"
#include "RainProvider.h"
#include "TemperatureHumidityProvider.h"
#include "WaterTemperatureProvider.h"
#include "WindDirectionProvider.h"
#include "WindSpeedProvider.h"
#include "Constants.h"
#include "Utils.h"
#include "Pair.h"
#include "SampleSet.h"
#include <SoftReset.h>
#include <SerialDebug.h>
#include <RingBufHelpers.h>
#include "Coord.h"
#include "RadioTransceiver.h"
#include "Sleep_n0m1.h"

class AnemioStation {
    public:
        AnemioStation();
        void setup(bool initialLaunch = true);
	    void loop();
		
		// Sets up devices and reports number that are offline (i.e. couldn't be setup).
		int setupProviders(int numberOfRetries = 3);

		// Checks health of devices and reports number that are offline.
		void healthCheck();

	private:
		SampleSet _sampleSet; // Stores the sampling information from all sensors.

		AmbientLightProvider _ambientLightProvider;
		BatteryInfoProvider _batteryInfoProvider;
		CompassAccelerometerProvider _compassAccelerometerProvider;
		PressureProvider _pressureProvider;
		RainProvider _rainProvider;
		TemperatureHumidityProvider _temperatureHumidityProvider;
		WaterTemperatureProvider _waterTemperatureProvider;
		WindDirectionProvider _windDirectionProvider;
		WindSpeedProvider _windSpeedProvider;

		RadioTransceiver _radioTransceiver;

		bool _online[Devices::TOTAL];
		unsigned long _lastCheck[ReadingChecks::ReadingChecks::TOTAL];
		unsigned long _radioLastTransmit;
		unsigned long _radioLastReceive;

		bool _sleeping = false;

		bool setupDevice_(Devices device);
		bool checkDeviceOnline_(Devices device);

		char formatBuff[RADIO_MAX_MESSAGE_LENGTH];

		Sleep sleep;
};

#endif