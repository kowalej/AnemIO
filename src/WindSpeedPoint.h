#pragma once

struct windspeedpoint {
	windspeedpoint(float v = NULL) : speedCorrected(NULL), sensorTemperature(NULL) {}
	windspeedpoint(float speed, float temperature) : speedCorrected(speed), sensorTemperature(temperature) {}

	float speedCorrected; // The corrected speed that was determined by the sensor temperature.
	float sensorTemperature; // The sensor temperature.
};