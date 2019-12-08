#include "RadioTransceiver.h"


bool RadioTransceiver::setup() {
	// Reset the radio.
	pinMode(RADIO_RESET_PIN, OUTPUT);
	digitalWrite(RADIO_RESET_PIN, HIGH);
	delayMicroseconds(100);
	digitalWrite(RADIO_RESET_PIN, LOW);
	delay(15);

	// Setup the radio.
	bool radioInit = _radio.initialize(RADIO_FREQUENCY, RADIO_STATION_NODE_ID, RADIO_NETWORK_ID);
	debugA("Radio initialized? %s", radioInit ? "true" : "false");

	// Set modes / calibrate.
	_radio.setHighPower(); // Set to high power since we are using RFM69HCW.
	_radio.rcCalibration(); // Perform this calibration to optimize for temperature variability.
	_radio.promiscuous(false); // Don't sniff all packets on network, since we are only ever sending messages anyways.
	_radio.setPowerLevel((int)(0.85f * 31)); // 85% (of 31).

	// Encryption.
	#ifdef RADIO_ENCRYPT
	_radio.encrypt(RADIO_ENCRYPT_KEY);
	#endif

	// Auto power can save battery, but the receiving station needs to enable ATC for this to setup correctly.
	// Right now the Raspberry PI Python library does not have this functionality.
	#ifdef RADIO_ENABLE_ATC
	_radio.enableAutoPower(RADIO_ATC_RSSI);
	#endif

	sleep();
	return radioInit;
}

void RadioTransceiver::sleep() {
	_radio.sleep();
}

void RadioTransceiver::wake() {
	_radio.receiveDone(); // This function will wake the device.
}

bool RadioTransceiver::sendMessageWithAutoWake(int command, const char* message, uint8_t retries) {
	wake();
	bool sent = sendMessage(command, message, retries);
	sleep();
	return sent;
}

bool RadioTransceiver::sendMessage(int command, const char* message, uint8_t retries) {
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH] = { '\0' };
	sprintf(formatBuff, "[%d]%s", command, message);
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), retries, RADIO_RETRY_WAIT_MS);
	return sent;
}

inline unsigned long getRelativeTimestamp(unsigned long timestamp, unsigned long baseTimestamp) {
	return timestamp - baseTimestamp;
}

// Sends a single sample.
bool RadioTransceiver::sendSample(unsigned long timestamp, const char* serializedValue, unsigned long baseTimestamp) {
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH];
	sprintf(formatBuff, "[%d]%lu%s", RadioCommands::SAMPLE_WRITE, getRelativeTimestamp(timestamp, baseTimestamp), serializedValue);
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	return sent;
}

//  Only sends a message when the radio buffer is full.
void RadioTransceiver::sendSampleCompact(unsigned long timestamp, const char* serializedValue, unsigned long baseTimestamp, char* sampleBuff, int& numSent, int& numSuccess, bool flush) {
	char buff[RADIO_MAX_MESSAGE_LENGTH];
	int newSampleLength = sprintf(buff, "[%d]%lu%s", RadioCommands::SAMPLE_WRITE, getRelativeTimestamp(timestamp, baseTimestamp), serializedValue);
	int currentBuffLength = strlen(sampleBuff);
	for (int i = currentBuffLength; i < currentBuffLength + newSampleLength; i++) {
		sampleBuff[i] = buff[i - currentBuffLength];
	}
	int sampleLength = strlen(sampleBuff);
	if (sampleLength >= RADIO_MAX_MESSAGE_LENGTH) {
		numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, sampleBuff, RADIO_MAX_MESSAGE_LENGTH, RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
		int charsRemaining = sampleLength - RADIO_MAX_MESSAGE_LENGTH;
		for (int i = 0; i < charsRemaining; i++) {
			sampleBuff[i] = sampleBuff[RADIO_MAX_MESSAGE_LENGTH + i];
		}
		for (int i = charsRemaining; i < sampleLength; i++) {
			sampleBuff[i] = null;
		}
	}

	if (flush) {
		int sampleLength = strlen(sampleBuff);
		if (sampleLength != 0) {
			int buffInd = 0;

			for (int i = 0; i < sampleLength; i++) {
				buff[buffInd] = sampleBuff[i];
				if (buffInd == RADIO_MAX_MESSAGE_LENGTH - 1 || i == sampleLength - 1) {
					numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, sampleBuff, RADIO_MAX_MESSAGE_LENGTH, RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
					buffInd = 0;
				}
			}
		}
		sampleBuff[0] = '\0';
	}
}

void groupPacketStartMessage(Sensors::Sensors sensor, const char* sampleCategory, const char* headerFormat, size_t numElements, unsigned long firstSampleTime, char* formatBuff) {
	// D: Device, C: Sample category, N: number of elements in upcoming series of samples, B: absolute value of first timestamp in upcoming set, T: current timestamp.
	sprintf(formatBuff, "D:%dC:%sF:%sN:%uB:%luT:%lu", sensor, sampleCategory, headerFormat, numElements, firstSampleTime, millis());
}

void groupPacketEndMessage(Sensors::Sensors sensor, const char* sampleCategory, char* formatBuff) {
	// D: Device, T: current timestamp.
	sprintf(formatBuff, "D:%dC:%sT:%lu", sensor, sampleCategory, millis());
}

Pair<int,int> RadioTransceiver::sendSamples(SampleSet& sampleSet) {
	int messageCount = 0;
	int sentMessages = 0;
	unsigned long sampleBaseTimestamp;
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH]; // Should only be filled to 54 chars if using for sending samples.
	char sampleBuff[RADIO_MAX_MESSAGE_LENGTH * 2];

	// Wake up the radio before sending this series of messages.
	wake();
	
	// Tell base station we are starting samples.
	sprintf(formatBuff, "T:%lu", millis());
	sentMessages += sendMessage(RadioCommands::SAMPLES_START, formatBuff); messageCount++;

	#pragma region Ambient Light Values.
	// Start ambient light samples.
	sampleBaseTimestamp = sampleSet.ambientLightSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::AMBIENT_LIGHT, "VALUES", "T|V", sampleSet.ambientLightSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;
	
	// Samples.
	while (!sampleSet.ambientLightSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.ambientLightSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.ambientLightSamples.isEmpty());
	}

	// End ambient light samples.
	groupPacketEndMessage(Sensors::AMBIENT_LIGHT, "VALUES", formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_END, formatBuff); messageCount++;
	#pragma endregion

	#pragma region Ambient Light State
	// Start ambient light state samples.
	sampleBaseTimestamp = sampleSet.ambientLightStateSample.first();
	groupPacketStartMessage(Sensors::AMBIENT_LIGHT, "STATE", "T|V", 1, sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	sentMessages += sendSample(sampleSet.ambientLightStateSample.first(), sampleSet.ambientLightStateSample.second().c_str(), sampleBaseTimestamp); messageCount++;

	// End ambient light state samples.
	groupPacketEndMessage(Sensors::AMBIENT_LIGHT, "STATE", formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_END, formatBuff); messageCount++;
	#pragma endregion
	
	#pragma region Compass XYZ.
	/* Disabling compass XYZ, since data is sort of useless.
	// Start compass XYZ samples.
	sampleBaseTimestamp = sampleSet.compassXYZSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::COMPASS, "XYZ", "T|V", sampleSet.compassXYZSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	while (!sampleSet.compassXYZSamples.isEmpty()) {
		Pair<unsigned long, coord> sample;
		sampleSet.compassXYZSamples.pull(&sample);
		snprintf(formatBuff, sizeof(formatBuff), "X: %d, Y: %d, Z: %d", sample.second().x, sample.second().y, sample.second().z);
		sendSampleCompact(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.compassXYZSamples.isEmpty());
	}

	// End compass XYZ samples.
	groupPacketEndMessage(Sensors::COMPASS, "XYZ", formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_END, formatBuff); messageCount++;
	*/
	#pragma endregion

	#pragma region Compass Heading.
	// Start compass heading samples.
	sampleBaseTimestamp = sampleSet.compassHeadingSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::COMPASS, "HEADING", "T|V", sampleSet.compassHeadingSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	while (!sampleSet.compassHeadingSamples.isEmpty()) {
		Pair<unsigned long, int> sample;
		sampleSet.compassHeadingSamples.pull(&sample);
		sentMessages += sendSample(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp); messageCount++;
	}

	// End compass heading samples.
	groupPacketEndMessage(Sensors::COMPASS, "HEADING", formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_END, formatBuff); messageCount++;
	#pragma endregion

	#pragma region Accelerometer XYZ.
	// Start accelerometer XYZ samples.
	sampleBaseTimestamp = sampleSet.accelerometerXYZSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::ACCELEROMETER, "XYZ", "T|X|Y|Z", sampleSet.accelerometerXYZSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	while (!sampleSet.accelerometerXYZSamples.isEmpty()) {
		Pair<unsigned long, coord> sample;
		sampleSet.accelerometerXYZSamples.pull(&sample);
		snprintf(formatBuff, sizeof(formatBuff), "%d|%d|%d", sample.second().x, sample.second().y, sample.second().z);
		sentMessages += sendSample(sample.first(), formatBuff, sampleBaseTimestamp); messageCount++;
	}

	// End accelerometer XYZ samples.
	groupPacketEndMessage(Sensors::ACCELEROMETER, "XYZ", formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_END, formatBuff); messageCount++;
	#pragma endregion

	//// Pressure.
	//send = send && sendMessage("Pressure started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureSamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
	//	sampleSet.pressureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure finished: time = " + millis());

	//// Pressure temperature.
	//send = send && sendMessage("Pressure temperature started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureTemperatureSamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
	//	sampleSet.pressureTemperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure temperature finished: time = " + millis());

	//// Pressure altitude.
	//send = send && sendMessage("Pressure altitude started: time = " + millis());
	//for (int i = 0; i < sampleSet.pressureAltitudeSamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
	//	sampleSet.pressureAltitudeSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Pressure altitude finished: time = " + millis());

	//// Rain.
	//send = send && sendMessage("Rain started: time = " + millis());
	//for (int i = 0; i < sampleSet.rainSamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
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
	//	Pair<unsigned long, float> sample;
	//	sampleSet.temperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Temperature finished: time = " + millis());

	//// Humidity.
	//send = send && sendMessage("Humidity started: time = " + millis());
	//for (int i = 0; i < sampleSet.humiditySamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
	//	sampleSet.humiditySamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Humidity finished: time = " + millis());

	//// Water temperature.
	//send = send && sendMessage("Water temperature started: time = " + millis());
	//for (int i = 0; i < sampleSet.waterTemperatureSamples.numElements(); i++) {
	//	Pair<unsigned long, float> sample;
	//	sampleSet.waterTemperatureSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Water temperature finished: time = " + millis());

	//// Wind direction.
	//send = send && sendMessage("Wind direction started: time = " + millis());
	//for (int i = 0; i < sampleSet.windDirectionSamples.numElements(); i++) {
	//	Pair<unsigned long, int> sample;
	//	sampleSet.windDirectionSamples.pull(&sample);
	//	send = send && sendSample(sample.first(), String(sample.second()));
	//}
	//send = send && sendMessage("Wind direction finished: time = " + millis());

	//// Wind speed.
	//send = send && sendMessage("Wind speed started: time = " + millis());
	//for (int i = 0; i < sampleSet.windSpeedSamples.numElements(); i++) {
	//	Pair<unsigned long, windspeedpoint> sample;
	//	sampleSet.windSpeedSamples.pull(&sample);
	//	snprintf(formatBuff, sizeof(formatBuff), "Speed: %d, Temperature: %d", sample.second().speedCorrected, sample.second().sensorTemperature);
	//	String value = formatBuff;
	//	send = send && sendSample(sample.first(), value.c_str());
	//}
	//send = send && sendMessage("Wind speed finished: time = " + millis());

	//send = send && sendMessage("Transmission finished: time = " + millis());
	
	sprintf(formatBuff, "T:%lu", millis());
	sentMessages += sendMessage(RadioCommands::SAMPLES_FINISH, formatBuff); messageCount++;

	// Put radio back to sleep to save power.
	sleep();

	return Pair<int, int>(sentMessages, messageCount);
}

