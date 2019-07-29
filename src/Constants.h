// Constants.h

#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

#define GET_BUFFER_SIZE(sensorRateHz, maxSendMs) (maxSendMs/1000 * sensorRateHz) 

namespace {
	const uint8_t MAX_SEND_INTERVAL_MS = 10000; // Maximum time between transmitting consecutive sample sets over the radio.

	// Pressure / temperature sensor.
	const uint8_t BMP280_IC2_ADDRESS = 0x76; // Should be set to 0x77 if pushing 5v to SDO.
	const uint8_t PRESSURE_UPDATE_RATE_HZ_NORMAL = 5; // Normal mode update rate of pressure sensor.
	uint8_t PRESSURE_UPDATE_RATE_HZ = PRESSURE_UPDATE_RATE_HZ_NORMAL;

	// Rain sensor.
	const uint8_t RAIN_UPDATE_RATE_HZ_NORMAL = 10; // Normal mode update rate of rain sensor.
	uint8_t RAIN_UPDATE_RATE_HZ = RAIN_UPDATE_RATE_HZ_NORMAL;
	const uint8_t RAIN_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the rain sensor (we will average the samples).
	const uint8_t RAIN_SENSOR_INPUT_PIN = A1; // Analog input pin for the rain sensor.

	// Temperature / humidity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL = 1; // Normal mode update rate of temperature/humdity sensor.
	uint8_t TEMPERATURE_HUMIDITY_uPDATE_RATE_HZ = TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL;
	const uint8_t TEMPERATURE_HUMIDITY_INPUT_PIN = 22; // Digital input pin for the temperature humdity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_FAULT_MAX = 25; // Consecutive read faults, before taking offline.

	// Calculate update "rate" in milliseconds from Hz.
	float UPDATE_RATE_MS(uint8_t updateRateHz) {
		return 1000.0f / (float)updateRateHz;
	}

	enum Devices {
		AMBIENT_LIGHT,
		COMPASS_ACCELEROMETER,
		PRESSURE,
		RAIN,
		TEMPERATURE_HUMIDITY,
		WIND_DIRECTIO,
		WIND_SPEED,
		TOTAL
	};
}

#endif
