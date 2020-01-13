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
	const uint16_t RADIO_SEND_INTERVAL_MS = 5000; // Maximum time between transmitting consecutive sample sets over the radio.
	const uint16_t RADIO_RECEIVE_INTERVAL_MS = 15000; // Maximum time between receiving commands over the radio.
	const uint16_t RADIO_RECEIVE_WAIT_MS = 500; // Default time to wait for command from base station.
	const uint16_t RADIO_SLEEP_MODE_RECEIVE_WAIT_MS = 2000; // How long check for wake message before sleeping again.

	const uint16_t SLEEP_MODE_SLEEP_TIME_MS = 15000; // How long to sleep before checking for message.

	// Ambient light sensor.
	const uint8_t AMBIENT_LIGHT_UPDATE_RATE_HZ = 1; // Update rate of light sensor.
	const uint8_t AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the ambient light sensor (we will average the samples).
	const uint8_t AMBIENT_LIGHT_SENSOR_INPUT_PIN = A2;
	const uint32_t AMBIENT_LIGHT_SENSOR_RESISTANCE_OHM = 100000; // This is the value of the internal resistor in the sensor.

	// Compass / accelerometer sensor.
	const uint8_t COMPASS_ACCELEROMETER_UPDATE_RATE_HZ = 2; // Update rate of compass / accelerometer sensor.

	// Pressure / temperature sensor.
	const uint8_t PRESSURE_UPDATE_RATE_HZ = 1; // Update rate of pressure sensor.
	const uint8_t BMP280_IC2_ADDRESS = 0x76; // Should be set to 0x77 if pushing 5v to SDO.

	// Rain sensor.
	const uint8_t RAIN_UPDATE_RATE_HZ = 2; // Update rate of rain sensor.
	const uint8_t RAIN_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the rain sensor (we will average the samples).
	const uint8_t RAIN_SENSOR_INPUT_PIN = A1; // Analog input pin for the rain sensor.

	// Temperature / humidity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ = 1; // Update rate of temperature/humdity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_INPUT_PIN = 22; // Digital input pin for the temperature humdity sensor.
	const uint8_t TEMPERATURE_HUMIDITY_FAULT_MAX = 25; // Consecutive read faults, before taking offline.

	// Wind direction sensor.
	const uint8_t WIND_DIRECTION_UPDATE_RATE_HZ = 1; // Update rate of wind direction sensor (rotary encoder).
	const uint8_t WIND_DIRECTION_FAULT_MAX = 25; // Consecutive read faults, before taking offline.
	const uint8_t WIND_DIRECTION_SENSOR_CLK_PIN = 7; // Use a PWM "clock" pin.
	const uint8_t WIND_DIRECTION_SENSOR_CS_PIN = 32; // Chip select (digital pin, out).
	const uint8_t WIND_DIRECTION_SENSOR_DO_PIN = 33; // Data pin (in).

	// Wind speed sensor.
	const uint8_t WIND_SPEED_UPDATE_RATE_HZ = 1; // Update rate of wind speed sensor.
	const uint8_t WIND_SPEED_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the wind speed sensor (we will average the samples).
	const uint8_t WIND_SPEED_SENSOR_SPEED_INPUT_PIN = A3;
	const uint8_t WIND_SPEED_SENSOR_TEMPERATURE_INPUT_PIN = A4;
	const float WIND_SPEED_SENSOR_ZERO_WIND_VOLTAGE = 1.340; // This may vary slightly from sensor to sensor.

	// Water temperature sensor.
	constexpr float WATER_TEMP_UPDATE_RATE_HZ = 0.25f; // Update rate of light sensor.
	const uint8_t WATER_TEMP_ANALOG_READ_SAMPLE_COUNT = 50; // Number of times we should do an analog read from the water temp sensor (we will average the samples).
	const uint8_t WATER_TEMP_SENSOR_INPUT_PIN = A5;
	const uint8_t WATER_TEMP_SENSOR_TRIGGER_PIN = 28; // Output pin to power the thermistor.
	const uint16_t WATER_TEMP_EXTERNAL_RESISTOR_OHMS = 10000; // The ohms for the resitor we bridged with the analog input.
	const uint16_t WATER_TEMP_THERMISTOR_NOMINAL_OHMS = 10000; // The nominal resistance at the nominal temperature of the thermistor (temperature probe).
	const uint16_t WATER_TEMP_THERMISTOR_NOMINAL_TEMPERATURE = 25; // The nominal temperature for the probe.
	const uint16_t WATER_TEMP_THERMISTOR_BETA_COEFFICIENT = 3950; // This is the beta coefficient used for the Steinhart calculation.
	
	// Radio settings.
	const uint8_t RADIO_CS_SLAVE_SELECT_PIN = 53; // Chip select CS (aka SS - slave select).
	const uint8_t RADIO_INTERRUPT_PIN = 2;
	const uint8_t RADIO_RESET_PIN = 49; // Manually reset the radio...

	const uint16_t RADIO_STATION_NODE_ID = 97; // This is our attached radio ID.
	const uint8_t RADIO_BASE_NODE_ID = 87; // This is the receiving radio ID.
	const uint8_t RADIO_NETWORK_ID = 223;
	const uint8_t RADIO_RETRY_NUM = 4;
	const uint8_t RADIO_RETRY_WAIT_MS = 40;
	const uint8_t RADIO_FREQUENCY = RF69_915MHZ;
	const int16_t RADIO_ATC_RSSI = -90;
	const char* RADIO_ENCRYPT_KEY = "J53Y25U5D8CE79NO";
	const int RADIO_MAX_MESSAGE_LENGTH = 61;
	const char* COMPACT_MESSAGES_START = "^^";
	const char* COMPACT_MESSAGES_END = "$$";

	enum Devices {
		AMBIENT_LIGHT = 0,
		COMPASS_ACCELEROMETER = 1,
		PRESSURE = 2,
		RAIN = 3,
		TEMPERATURE_HUMIDITY = 4,
		WATER_TEMPERATURE = 5,
		WIND_DIRECTION = 6,
		WIND_SPEED = 7,
		TOTAL = 8
	};

	const char* DeviceNames[] = {
		"AMBIENT_LIGHT",
		"COMPASS_ACCELEROMETER",
		"PRESSURE",
		"RAIN",
		"TEMPERATURE_HUMIDITY",
		"WATER_TEMPERATURE",
		"WIND_DIRECTION",
		"WIND_SPEED"
	};

	namespace SensorCategory {
		enum SensorCategory {
			AMBIENT_LIGHT_VALUES = 0,
			AMBIENT_LIGHT_STATE = 1,
			COMPASS_XYZ = 2,
			COMPASS_HEADING = 3,
			ACCELEROMETER_XYZ = 4,
			PRESSURE_PRESSURE = 5,
			PRESSURE_TEMPERATURE = 6,
			PRESSURE_ALTITUDE = 7,
			RAIN_VALUES = 8,
			RAIN_STATE = 9,
			TEMPERATURE = 10,
			HUMIDITY = 11,
			WATER_TEMPERATURE = 12,
			WIND_DIRECTION = 13,
			WIND_SPEED = 14
		};
	}

	enum RadioCommands {
		SETUP_START = 1,
		REPORT_SETUP_STATE = 2,
		SETUP_FINISH = 3,
		REPORT_ONLINE_STATE = 4,
		SAMPLES_START = 5,
		SAMPLE_GROUP_DIVIDER = 6,
		SAMPLE_WRITE = 7,
		SAMPLES_FINISH = 8,
		SLEEP = 9,
		WAKE = 10,
		RESTART = 11
	};
}

#endif
