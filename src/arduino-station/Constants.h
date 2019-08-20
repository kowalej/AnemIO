// Constants.h

#ifndef CONSTANTS_H
#define CONSTANTS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <RFM69.h>

#define GET_BUFFER_SIZE(sensorRateHz, maxSendMs) (size_t(maxSendMs/1000 * sensorRateHz)) 

namespace {
	const uint16_t MAX_SEND_INTERVAL_MS = 8000; // Maximum time between transmitting consecutive sample sets over the radio.
	const uint16_t SCREEN_UPDATE_RATE_HZ = 1; // Update rate of the onboard screen.

	// Pressure / temperature sensor.
	const uint8_t PRESSURE_UPDATE_RATE_HZ_NORMAL = 2; // Normal mode update rate of pressure sensor.
	uint8_t PRESSURE_UPDATE_RATE_HZ = PRESSURE_UPDATE_RATE_HZ_NORMAL;
	const uint8_t BMP280_IC2_ADDRESS = 0x76; // Should be set to 0x77 if pushing 5v to SDO.

	// Rain sensor.
	const uint8_t RAIN_UPDATE_RATE_HZ_NORMAL = 8; // Normal mode update rate of rain sensor.
	uint8_t RAIN_UPDATE_RATE_HZ = RAIN_UPDATE_RATE_HZ_NORMAL;
	const uint8_t RAIN_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the rain sensor (we will average the samples).
	const uint8_t RAIN_SENSOR_INPUT_PIN = A1; // Analog input pin for the rain sensor.

	// Temperature / humidity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL = 1; // Normal mode update rate of temperature/humdity sensor.
	uint8_t TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ = TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL;
	const uint8_t TEMPERATURE_HUMIDITY_INPUT_PIN = 22; // Digital input pin for the temperature humdity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_FAULT_MAX = 25; // Consecutive read faults, before taking offline.

	// Ambient light sensor.
	const uint8_t AMBIENT_LIGHT_UPDATE_RATE_HZ_NORMAL = 2; // Normal mode update rate of light sensor.
	uint8_t AMBIENT_LIGHT_UPDATE_RATE_HZ = AMBIENT_LIGHT_UPDATE_RATE_HZ_NORMAL;
	const uint8_t AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the ambient light sensor (we will average the samples).
	const uint8_t AMBIENT_LIGHT_SENSOR_INPUT_PIN = A2;
	const uint32_t AMBIENT_LIGHT_SENSOR_RESISTANCE_OHM = 100000; // This is the value of the internal resistor in the sensor.

	// Compass / accelerometer sensor.
	const uint8_t COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL = 8; // Normal mode update rate of compass / accelerometer sensor.
	uint8_t COMPASS_ACCELEROMETER_UPDATE_RATE_HZ = COMPASS_ACCELEROMETER_UPDATE_RATE_HZ_NORMAL;

	// Wind speed sensor.
	const uint8_t WIND_SPEED_UPDATE_RATE_HZ_NORMAL = 4; // Normal mode update rate of wind speed sensor.
	uint8_t WIND_SPEED_UPDATE_RATE_HZ = WIND_SPEED_UPDATE_RATE_HZ_NORMAL;
	const uint8_t WIND_SPEED_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the wind speed sensor (we will average the samples).
	const uint8_t WIND_SPEED_SENSOR_SPEED_INPUT_PIN = A3;
	const uint8_t WIND_SPEED_SENSOR_TEMPERATURE_INPUT_PIN = A4;
	const float WIND_SPEED_SENSOR_ZERO_WIND_VOLTAGE = 1.340; // This may vary slightly from sensor to sensor.

	// Wind direction sensor.
	const uint8_t WIND_DIRECTION_UPDATE_RATE_HZ_NORMAL = 1; // Normal mode update rate of wind direction sensor (rotary encoder).
	const uint8_t WIND_DIRECTION_UPDATE_RATE_HZ = WIND_DIRECTION_UPDATE_RATE_HZ_NORMAL;
	const uint8_t WIND_DIRECTION_FAULT_MAX = 25; // Consecutive read faults, before taking offline.
	const uint8_t WIND_DIRECTION_SENSOR_CLK_PIN = 7; // Use a PWM "clock" pin.
	const uint8_t WIND_DIRECTION_SENSOR_CS_PIN = 32; // Chip select (digital pin, out).
	const uint8_t WIND_DIRECTION_SENSOR_DO_PIN = 33; // Data pin (in).

	// Water temperature sensor.
	constexpr float WATER_TEMP_UPDATE_RATE_HZ_NORMAL = 0.25f; // Normal mode update rate of light sensor.
	float WATER_TEMP_UPDATE_RATE_HZ = WATER_TEMP_UPDATE_RATE_HZ_NORMAL;
	const uint8_t WATER_TEMP_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the water temp sensor (we will average the samples).
	const uint8_t WATER_TEMP_SENSOR_INPUT_PIN = A5;
	const uint8_t WATER_TEMP_SENSOR_TRIGGER_PIN = 24; // Output pin to power the thermistor.
	const uint16_t WATER_TEMP_EXTERNAL_RESISTOR_OHMS = 10000; // The ohms for the resitor we bridged with the analog input.
	const uint16_t WATER_TEMP_THERMISTOR_NOMINAL_OHMS = 10000; // The nominal resistance at the nominal temperature of the thermistor (temperature probe).
	const uint16_t WATER_TEMP_THERMISTOR_NOMINAL_TEMPERATURE = 25; // The nominal temperature for the probe.
	const uint16_t WATER_TEMP_THERMISTOR_BETA_COEFFICIENT = 3950; // This is the beta coefficient used for the Steinhart calculation.
	
	// Radio settings.
	const uint8_t RADIO_CS_SLAVE_SELECT_PIN = 48;
	const uint8_t RADIO_INTERRUPT_PIN = 2;
	const uint8_t RADIO_RESET_PIN = 49; // Manually reset the radio...

	const uint8_t RADIO_STATION_NODE_ID = 7;
	const uint8_t RADIO_BASE_NODE_ID = 8;
	const uint8_t RADIO_NETWORK_ID = 100;
	const uint8_t RADIO_RETRY_NUM = 3;
	const uint8_t RADIO_RETRY_WAIT_MS = 40;

	const uint8_t RADIO_FREQUENCY = RF69_915MHZ;

	const int8_t RADIO_ATC_RSSI = -75;

	enum Devices {
		AMBIENT_LIGHT,
		COMPASS_ACCELEROMETER,
		PRESSURE,
		RAIN,
		TEMPERATURE_HUMIDITY,
		WATER_TEMPERATURE,
		WIND_DIRECTION,
		WIND_SPEED,
		TOTAL
	};
}

#endif