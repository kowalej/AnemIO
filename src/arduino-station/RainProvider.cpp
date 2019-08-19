#include "RainProvider.h"

RainProvider::RainProvider() { }

bool RainProvider::setup()
{
	pinMode(RAIN_SENSOR_INPUT_PIN, INPUT);
	_isOnline = true;
	return _isOnline;
}

float RainProvider::getRainValue()
{
	float total = 0;
	for (int i = 0; i < RAIN_ANALOG_READ_SAMPLE_COUNT; i++) {
		total += analogRead(RAIN_SENSOR_INPUT_PIN);
	}
	return total / RAIN_ANALOG_READ_SAMPLE_COUNT;
}

const String RainProvider::getRainState(SampleSet& samples)
{
	int sampleCount = samples.rainSamples.numElements();
	float average = 0;
	float variance = 0;
	// Calculate the average.
	for (int i = 0; i < sampleCount; i++) {
		average += samples.rainSamples.peek(i)->second();
	}
	average = average / sampleCount;

	// Calculate the variance.
	for (int i = 0; i < sampleCount; i++) {
		variance += sq(samples.rainSamples.peek(i)->second() - average);
	}
	variance = variance / sampleCount;

	if (variance > 0) return "fuckyou";
	return "fuck you fo sho";
}
