#include "AmbientLightProvider.h"

AmbientLightProvider::AmbientLightProvider() { }

bool AmbientLightProvider::setup()
{
	pinMode(AMBIENT_LIGHT_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float AmbientLightProvider::getAmbientLightValue()
{
	float total = 0;
	for (int i = 0; i < AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(AMBIENT_LIGHT_SENSOR_INPUT_PIN);
	}
	return total / AMBIENT_LIGHT_ANALOG_READ_SAMPLE_COUNT;
}

float AmbientLightProvider::getLux(float value) {
	float voltage = value * (5.0f / 1024.0f);

	// Get current.
	float current = voltage / (float)AMBIENT_LIGHT_SENSOR_RESISTANCE_OHM * 1000000.0f; // 100K resistor, then multiply to get from amps to uA.

	// Get lux.
	return pow(10, (current / 10)); // Logarithmic relation according to datasheet: https://www.mouser.ca/datasheet/2/365/GA1A1S201WP_Spec-184717.pdf.
}

const String AmbientLightProvider::getAmbientLightState(SampleSet &samples)
{
	// Calculate the average.
	int sampleCount = samples.ambientLightSamples.numElements();
	float average = 0;
	for (int i = 0; i < sampleCount; i++) {
		average += samples.ambientLightSamples.peek(i)->second();
	}
	average = average / sampleCount;
	debugD("Ambient light average: %s", String(average).c_str());

	// Get lux and use lookup below to determine overall state.
	float lux = getLux(average);

	debugD("Lux: %s", String(lux).c_str());

	if (lux > 25000) {
		return "Very Sunny";
	}
	else if (lux > 10000) {
		return "Sunny";
	}
	else if (lux > 5000) {
		return "Partly Sunny";
	}
	else if (lux >= 1000 && lux < 5000) {
		return "Partly Cloudy";
	}
	else if (lux < 1000 && lux > 200) {
		return "Cloudy";
	}
	else if (lux <= 200 && lux > 50) {
		return "Very Cloud";
	}
	else if (lux <= 50 && lux > 10) {
		return "Dusk";
	}
	else if (lux <= 10 && lux > 5) {
		return "Twilight";
	}
	else if (lux <= 5) {
		return "Night";
	}
}
