// SampleSet.h

#ifndef SAMPLE_SET_h
#define SAMPLE_SET_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Constants.h"
#include "Pair.h"
#include <RingBufHelpers.h>
#include <RingBufCPP.h>

class SampleSet {
	public:
		SampleSet();
		
		// Pressure (Temperature) and Altitude.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureTemperatureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> pressureAltitudeSamples;

		// Temperature / Humidity.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> temperatureSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> humiditySamples;

		// Rain.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(RAIN_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> rainSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(RAIN_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> rainStateSamples;

		// Ambient Light.
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(AMBIENT_LIGHT_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> ambientLightSamples;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(AMBIENT_LIGHT_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> ambientLightStateSamples;

		// Compass / Accelerometer
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> compassSamplesx;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> accelerometerSamplesx;

		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> compassSamplesy;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> accelerometerSamplesy;

		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> compassSamplesZ;
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> accelerometerSamplesz;

		// Wind Speed
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(WIND_SPEED_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> windSpeedSamples;

		// Wind Direction
		RingBufCPP<Pair<int, float>, GET_BUFFER_SIZE(WIND_DIRECTION_UPDATE_RATE_HZ_NORMAL, MAX_SEND_INTERVAL_MS)> windDirectionSamples;
};

#endif

