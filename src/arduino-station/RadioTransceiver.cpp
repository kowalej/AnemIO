#include "RadioTransceiver.h"

bool RadioTransceiver::setup()
{
	// Reset the radio.
	pinMode(RADIO_RESET_PIN, OUTPUT);
	digitalWrite(RADIO_RESET_PIN, LOW);
	delayMicroseconds(100);
	digitalWrite(RADIO_RESET_PIN, HIGH);
	delayMicroseconds(100);
	digitalWrite(RADIO_RESET_PIN, LOW);
	delay(5);

	// Setup the radio.
	bool radioInit = _radio.initialize(RADIO_FREQUENCY, RADIO_STATION_NODE_ID, RADIO_NETWORK_ID);
	debugA("Radio initialized? %s", radioInit ? "true" : "false");
	_radio.setHighPower(); // Call set high power since we are using RFM69HCW.
	#ifdef RADIO_ENABLE_ATC
	_radio.enableAutoPower(RADIO_ATC_RSSI);
	#endif
	return radioInit;

	//attachInterrupt(digitalPinToInterrupt(RADIO_INTERRUPT_PIN), LOW, mode);
}

void RadioTransceiver::sleep() {
	_radio.sleep();
}

void RadioTransceiver::wake() {
	_radio.receiveDone();
}

bool RadioTransceiver::sendMessage(const char* message)
{
	wake();
	bool send = _radio.sendWithRetry(RADIO_BASE_NODE_ID, message, strlen(message), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	sleep();
	return send;
}

bool RadioTransceiver::sendSample(long timestamp, String serializedValue) {
	String message = "T: " + String(timestamp) + "V: " + serializedValue;
	_radio.sendWithRetry(RADIO_BASE_NODE_ID, message.c_str(), strlen(message.c_str()), RADIO_RETRY_WAIT_MS);
}

bool RadioTransceiver::sendSamples(SampleSet &sampleSet)
{
	wake();
	bool send = false;
	
	send = sendMessage(("Transmission started: time = " + millis()));

	send &= sendMessage("Ambient Light");
	for (int i = 0; i < sampleSet.ambientLightSamples.numElements(); i++) {
		Pair<long, float> sample;
		sampleSet.ambientLightSamples.pull(&sample);
		send &= sendSample(sample.first(), String(sample.second()));
	}

	send &= sendMessage(("Transmission complete: time = " + millis()));

	sleep();
	return send;
}
