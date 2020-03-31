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
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(AMBIENT_LIGHT_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> ambientLightSamples;
		RingBufCPP<Pair<unsigned long, String>, GET_BUFFER_SIZE(AMBIENT_LIGHT_STATE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> ambientLightStateSamples; // Ambient light state (aggregated).

		// Battery Level.
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(BATTERY_INFO_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> batteryLevelSamples;

		// Compass / Accelerometer.
		RingBufCPP<Pair<unsigned long, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> compassXYZSamples;
		RingBufCPP<Pair<unsigned long, coord>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> accelerometerXYZSamples;
		RingBufCPP<Pair<unsigned long, int>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> compassHeadingSamples;

		// Pressure (Temperature) and Altitude.
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureSamples;
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureTemperatureSamples;
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> pressureAltitudeSamples;

		// Rain.
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(RAIN_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> rainSamples;
		RingBufCPP<Pair<unsigned long, String>, GET_BUFFER_SIZE(RAIN_STATE_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> rainStateSamples; // Rain state (aggregated).

		// Temperature / Humidity.
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> temperatureSamples;
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> humiditySamples;

		// Water Temperature.
		RingBufCPP<Pair<unsigned long, float>, GET_BUFFER_SIZE(WATER_TEMP_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> waterTemperatureSamples;

		// Wind Direction.
		RingBufCPP<Pair<unsigned long, int>, GET_BUFFER_SIZE(WIND_DIRECTION_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> windDirectionSamples;

		// Wind Speed.
		RingBufCPP<Pair<unsigned long, windspeedpoint>, GET_BUFFER_SIZE(WIND_SPEED_UPDATE_RATE_HZ, RADIO_SEND_INTERVAL_MS)> windSpeedSamples;
};

#endif

