// Utils.h

#ifndef UTILS_H
#define UTILS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

namespace {
	// Calculate update "rate" in milliseconds from Hz.
	float UPDATE_RATE_MS(float updateRateHz) {
		return (1000.0f / (float)updateRateHz);
	}

	unsigned long TIME_DELTA(unsigned long lastCheck) {
		return abs(millis() - lastCheck);
	}
}

#endif