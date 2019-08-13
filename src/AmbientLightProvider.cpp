#include "AmbientLightProvider.h"
#include "SerialDebug.h"

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
	voltage = voltage / 100000.0f * 1000000; // 100K resistor, then multiply to get amps to uA.

	// Get lux.
	voltage = pow(10, (voltage / 10)); // Logarithmic relation according to datasheet: https://www.mouser.ca/datasheet/2/365/GA1A1S201WP_Spec-184717.pdf.

	return voltage;
}

const String AmbientLightProvider::getAmbientLightState(SampleSet &samples)
{
	int sampleCount = samples.ambientLightSamples.numElements();
	float average = 0;
	// Calculate the average.
	for (int i = 0; i < sampleCount; i++) {
		average += samples.ambientLightSamples.peek(i)->second();
	}
	average = average / sampleCount;
	debugA("Ambi average: %s", String(average).c_str());

	float lux = getLux(average);

	debugA("Lux: %s", String(lux).c_str());

	if (lux > 25000) {
		return "Very Sunny";
	}
	if (lux > 10000) {
		return "Sunny";
	}
	if (lux > 5000) {
		return "Party Sunny";
	}
	if (lux >= 1000 && lux < 5000) {
		return "Party Cloudy";
	}
	if (lux < 1000 && lux > 200) {
		"Cloudy";
	}
	if (lux <= 200 && lux > 50) {
		"Very Cloud";
	}
	if (lux <= 50 && lux > 10) {
		"Dusk";
	}
	if (lux <= 10 && lux > 5) {
		"Twilight";
	}
	if (lux <= 5) {
		"Night";
	}
}
