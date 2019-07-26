// AnemioStation.h

#ifndef ANEMIO_STATION_H
#define ANEMIO_STATION_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <PrintEx.h>
#include <Wire.h>
#include "PressureProvider.h"
#include "RainProvider.h"
#include "TemperatureHumidityProvider.h"

class AnemioStation {
    public:
        AnemioStation();
        void setup();
	    void loop();

	private:
		PrintEx _serial; //Wrap the Serial object in a PrintEx interface.
		PressureProvider _pressureProvider;
		RainProvider _rainProvider;
		TemperatureHumidityProvider _temperatureHumidityProvider;
};

#endif