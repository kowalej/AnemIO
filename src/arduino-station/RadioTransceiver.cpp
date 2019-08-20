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

bool RadioTransceiver::sendMessage(const char* message)
{
	return _radio.sendWithRetry(RADIO_BASE_NODE_ID, message, strlen(message), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
}
