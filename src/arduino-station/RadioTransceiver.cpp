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
	_radio.receiveDone(); // This function will wake the device.
}

bool RadioTransceiver::sendMessageWithAutoWake(const char* message)
{
	wake();
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, message, strlen(message), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	sleep();
	return sent;
}

bool RadioTransceiver::sendMessage(const char* message)
{
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, message, strlen(message), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	return sent;
}

bool RadioTransceiver::sendSample(long timestamp, String serializedValue) {
	String message = "T: " + String(timestamp) + "V: " + serializedValue;
	const char* messageC = message.c_str();
	bool wasSent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, messageC, strlen(messageC), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	return wasSent;
}

bool RadioTransceiver::sendSamples(SampleSet &sampleSet)
{
	bool send = false;
	char formatBuff[50];

	sprintf(formatBuff, "Transmission started: time = %lu", millis());
	send = sendMessage(formatBuff);

	// Ambient light.
	//send = send && sendMessage("Ambient light started: time = " + millis());
	/*for (int i = 0; i < sampleSet.ambientLightSamples.numElements(); i++) {
		Pair<long, float> sample;
		sampleSet.ambientLightSamples.pull(&sample);
		send = send && sendSample(sample.first(), String(sample.second()));
	}*/
	//send = send && sendMessage("Ambient light finished: time = " + millis());

	//// Ambient light state.
	//send = send && sendMessage("Ambient light state started: time = " + millis());
	//send = send && sendSample(sampleSet.ambientLightStateSample.first(), sampleSet.ambientLightStateSample.second());
	//send = send && sendMessage("Ambient light state finished: time = " + millis());

	//// Compass XYZ.
	//send = send && sendMessage("Compass XYZ started: time = " + millis());
	//for (int i = 0; i < sampleSet.compassXYZSamples.numElements(); i++) {
	//	Pair<long, coord> sample;
	//	sampleSet.compassXYZSamples.pull(&sample);
	//	snprintf(formatBuff, sizeof(formatBuff), "X: %d, Y: %d, Z: %d", sample.second().x, sample.second().y, sample.second().z);
	//	String value = formatBuff;
	//	send = send && sendSample(sample.first(), value.c_str());
	//}
	//send = send && sendMessage("Compass XYZ finished: time = " + millis());

	//// Accelerometer XYZ.
	//send = send && sendMessage("Accelerometer XYZ started: time = " + millis());
	//for (int i = 0; i < sampleSet.accelerometerXYZSamples.numElements(); i++) {
	//	Pair<long, coord> sample;
	//	sampleSet.accelerometerXYZSamples.pull(&sample);
	//	snprintf(formatBuff, sizeof(formatBuff), "X: %d, Y: %d, Z: %d", sample.second().x, sample.second().y, sample.second().z);
	//	String value = formatBuff;
	//	send = send && sendSample(sample.first(), value.c_str());
	//}
	//send = send && sendMessage("Accelerometer XYZ finished: time = " + millis());

	//// Compass heading.
	//send = send && sendMessage("Compass heading started: time = " + millis());
	//for (int i = 0; i < sampleSet.compassHeadingSamples.numElements(); i++) {
	//	Pair<long, int> sample;
	//	sampleSet.compassHeadingSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Compass heading finished: time = " + millis());

	//// Pressure.
	//send = send && sendMessage("Pressure started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.pressureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure finished: time = " + millis());

	//// Pressure temperature.
	//send = send && sendMessage("Pressure temperature started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureTemperatureSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.pressureTemperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure temperature finished: time = " + millis());

	//// Pressure altitude.
	//send = send && sendMessage("Pressure altitude started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureAltitudeSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.pressureAltitudeSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure altitude finished: time = " + millis());

	//// Rain.
	//send = send && sendMessage("Rain started: time = " + millis());
	//for (int i = 0; i < sampleSet.rainSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.rainSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Rain finished: time = " + millis());

	//// Rain state.
	//send = send && sendMessage("Rain State Started: Time = " + millis());
	//send = send && sendSample(sampleSet.rainStateSample.first(), sampleSet.rainStateSample.second());
	//send = send && sendMessage("Rain state finished: time = " + millis());

	//// Temperature.
	//send = send && sendMessage("Temperature started: time = " + millis());
	//for (int i = 0; i < sampleSet.temperatureSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.temperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Temperature finished: time = " + millis());

	//// Humidity.
	//send = send && sendMessage("Humidity started: time = " + millis());
	//for (int i = 0; i < sampleSet.humiditySamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.humiditySamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Humidity finished: time = " + millis());

	//// Water temperature.
	//send = send && sendMessage("Water temperature started: time = " + millis());
	//for (int i = 0; i < sampleSet.waterTemperatureSamples.numElements(); i++) {
	//	Pair<long, float> sample;
	//	sampleSet.waterTemperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Water temperature finished: time = " + millis());

	//// Wind direction.
	//send = send && sendMessage("Wind direction started: time = " + millis());
	//for (int i = 0; i < sampleSet.windDirectionSamples.numElements(); i++) {
	//	Pair<long, int> sample;
	//	sampleSet.windDirectionSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Wind direction finished: time = " + millis());

	//// Wind speed.
	//send = send && sendMessage("Wind speed started: time = " + millis());
	//for (int i = 0; i < sampleSet.windSpeedSamples.numElements(); i++) {
	//	Pair<long, windspeedpoint> sample;
	//	sampleSet.windSpeedSamples.pull(&sample);
	//	snprintf(formatBuff, sizeof(formatBuff), "Speed: %d, Temperature: %d", sample.second().speedCorrected, sample.second().sensorTemperature);
	//	String value = formatBuff;
	//	send = send && sendSample(sample.first(), value.c_str());
	//}
	//send = send && sendMessage("Wind speed finished: time = " + millis());

	//send = send && sendMessage("Transmission finished: time = " + millis());
	return send;
}

