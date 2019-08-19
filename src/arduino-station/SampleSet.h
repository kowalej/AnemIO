// SampleSet.h

#ifndef SAMPLE_SET_h
#define SAMPLE_SET_h

#include "Constants.h"
#include "Pair.h"
#include <RingBufHelpers.h>
#include <RingBufCPP.h>
#include "Coord.h"
#include "WindSpeedPoint.h"

class SampleSet {
	public:
		SampleSet();
		
		// Ambient Light.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(AMBIENT_LIGHT_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> ambientLightSamples;

		// Compass / Accelerometer.
		RingBufCPP<Pair<int, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> compassXYZ;
		RingBufCPP<Pair<int, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> accelerometerXYZ;
		RingBufCPP<Pair<int, int>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> compassHeading;

		// Pressure (Temperature) and Altitude.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureTemperatureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureAltitudeSamples;

		// Rain.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(RAIN_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> rainSamples;

		// Temperature / Humidity.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> temperatureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> humiditySamples;

		// Water Temperature.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(WATER_TEMP_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> waterTemperatureSamples;

		// Wind Direction.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(WIND_DIRECTION_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> windDirectionSamples;

		// Wind Speed.
		RingBufCPP<Pair<int, windspeedpoint>, GET_BUFFER_SIZE(WIND_SPEED_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> windSpeedSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(WIND_SPEED_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> windAmbientTemperatureSamples;
};

#endif

