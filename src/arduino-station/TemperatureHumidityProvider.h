// TemperatureHumidityProvider.h

#ifndef TEMPERATURE_HUMIDITY_PROVIDER_H
#define TEMPERATURE_HUMIDITY_PROVIDER_H

#include "IDataProvider.h"
#include "Constants.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SerialDebug.h>

class TemperatureHumidityProvider : public IDataProvider
{
	public:
		TemperatureHumidityProvider();
		bool setup();
		float getTemperature();
		float getHumidity();

	private:
		DHT _temperatureHumditySensor;
		int _numConsecutiveFaultsTemperature = 0;
		int _numConsecutiveFaultsHumidity = 0;
		void checkFaults();
};

#endif
