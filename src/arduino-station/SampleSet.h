// SampleSet.h

#ifndef SAMPLE_SET_H
#define SAMPLE_SET_H

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
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(AMBIENT_LIGHT_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> ambientLightSamples;
		Pair<long, String> ambientLightStateSample; // Ambient light state (aggregate each send).

		// Compass / Accelerometer.
		RingBufCPP<Pair<long, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> compassXYZSamples;
		RingBufCPP<Pair<long, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> accelerometerXYZSamples;
		RingBufCPP<Pair<long, int>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> compassHeadingSamples;

		// Pressure (Temperature) and Altitude.
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureSamples;
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureTemperatureSamples;
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureAltitudeSamples;

		// Rain.
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(RAIN_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> rainSamples;
		Pair<long, String> rainStateSample; // Rain state (aggregate each send).

		// Temperature / Humidity.
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> temperatureSamples;
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> humiditySamples;

		// Water Temperature.
		RingBufCPP<Pair<long, float>, GET_BUFFER_SIZE(WATER_TEMP_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> waterTemperatureSamples;

		// Wind Direction.
		RingBufCPP<Pair<long, int>, GET_BUFFER_SIZE(WIND_DIRECTION_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> windDirectionSamples;

		// Wind Speed.
		RingBufCPP<Pair<long, windspeedpoint>, GET_BUFFER_SIZE(WIND_SPEED_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> windSpeedSamples;
};

#endif

