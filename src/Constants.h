#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

namespace {
    const uint8_t PRESSURE_UPDATE_RATE_HZ = 40;
	const uint8_t BMP280_IC2_ADDRESS = 0x76; // Should be set to 0x77 if pushing 5v to SDO.
	const uint8_t RAIN_SENSOR_INPUT_PIN = 16;
}

#endif
