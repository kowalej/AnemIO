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
	// [<command>]<time>|<value>.
	sprintf(formatBuff, "[%d]%lu|%s", RadioCommands::SAMPLE_WRITE, getRelativeTimestamp(timestamp, baseTimestamp), serializedValue);
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	return sent;
}

// Only sends a message when the radio buffer is full.
void RadioTransceiver::sendSampleCompact(unsigned long timestamp, const char* serializedValue, unsigned long baseTimestamp, char* sampleBuff, int& numSent, int& numSuccess, bool flush) {
	char buff[RADIO_MAX_MESSAGE_LENGTH];
	// [<command>]<time>|<value>;
	int currentBuffLength = strlen(sampleBuff);
	int newSampleLength = sprintf(buff, "[%d]%lu|%s", RadioCommands::SAMPLE_WRITE, getRelativeTimestamp(timestamp, baseTimestamp), serializedValue);
	strncat(sampleBuff, buff, newSampleLength);
	int sampleLength = currentBuffLength + newSampleLength;
	if (sampleLength >= RADIO_MAX_MESSAGE_LENGTH) {
		numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, sampleBuff, RADIO_MAX_MESSAGE_LENGTH, RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
		int charsRemaining = sampleLength - RADIO_MAX_MESSAGE_LENGTH;
		for (int i = 0; i < charsRemaining; i++) {
			sampleBuff[i] = sampleBuff[RADIO_MAX_MESSAGE_LENGTH + i];
		}
		sampleBuff[charsRemaining] = null;
	}

	if (flush) {
		int sampleLength = strlen(sampleBuff);
		if (sampleLength != 0) {
			int buffInd = 0;

			for (int i = 0; i < sampleLength; i++) {
				buff[buffInd] = sampleBuff[i];
				buffInd++;
				if (buffInd == RADIO_MAX_MESSAGE_LENGTH - 1 || i == sampleLength - 1) {
					numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, sampleBuff, strlen(buff), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
					buffInd = 0;
					for (int i = 0; i < sizeof(buff); i++) {
						buff[i] = null;
					}
				}
			}
		}
		sampleBuff[0] = null;
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
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH] = { null }; // Should only be filled to 54 chars if using for sending samples.
	char sampleBuff[RADIO_MAX_MESSAGE_LENGTH * 2] = { null };

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
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.ambientLightSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Ambient Light State
	// Start ambient light state samples.
	sampleBaseTimestamp = sampleSet.ambientLightStateSample.first();
	groupPacketStartMessage(Sensors::AMBIENT_LIGHT, "STATE", "T|V", 1, sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	sentMessages += sendSample(sampleSet.ambientLightStateSample.first(), sampleSet.ambientLightStateSample.second().c_str(), sampleBaseTimestamp); messageCount++;
	#pragma endregion
	
	#pragma region Compass XYZ.
	///* Disabling compass XYZ, since data is sort of useless.
	// Start compass XYZ samples.
	sampleBaseTimestamp = sampleSet.compassXYZSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::COMPASS, "XYZ", "T|X|Y|Z", sampleSet.compassXYZSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.compassXYZSamples.isEmpty()) {
		Pair<unsigned long, coord> sample;
		sampleSet.compassXYZSamples.pull(&sample);
		snprintf(formatBuff, sizeof(formatBuff), "%s|%s|%s", String(sample.second().x, 2).c_str(), String(sample.second().y, 2).c_str(), String(sample.second().z, 2).c_str());
		sendSampleCompact(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.compassXYZSamples.isEmpty());
	}
	//*/
	#pragma endregion

	#pragma region Compass Heading.
	// Start compass heading samples.
	sampleBaseTimestamp = sampleSet.compassHeadingSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::COMPASS, "HEADING", "T|V", sampleSet.compassHeadingSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.compassHeadingSamples.isEmpty()) {
		Pair<unsigned long, int> sample;
		sampleSet.compassHeadingSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.compassHeadingSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Accelerometer XYZ.
	// Start accelerometer XYZ samples.
	sampleBaseTimestamp = sampleSet.accelerometerXYZSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::ACCELEROMETER, "XYZ", "T|X|Y|Z", sampleSet.accelerometerXYZSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.accelerometerXYZSamples.isEmpty()) {
		Pair<unsigned long, coord> sample;
		sampleSet.accelerometerXYZSamples.pull(&sample);
		snprintf(formatBuff, sizeof(formatBuff), "%s|%s|%s", String(sample.second().x, 2).c_str(), String(sample.second().y, 2).c_str(), String(sample.second().z, 2).c_str());
		sendSampleCompact(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.accelerometerXYZSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Pressure.
	// Start pressure samples.
	sampleBaseTimestamp = sampleSet.pressureSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::PRESSURE, "PRESSURE", "T|V", sampleSet.pressureSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.pressureSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.pressureSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.pressureSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Pressure sensor temperature.
	// Start pressure sensor temperature samples.
	sampleBaseTimestamp = sampleSet.pressureTemperatureSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::PRESSURE, "TEMPERATURE", "T|V", sampleSet.pressureTemperatureSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.pressureTemperatureSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.pressureTemperatureSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.pressureTemperatureSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Pressure altitude estimate.
	// Start pressure altitude estimate samples.
	sampleBaseTimestamp = sampleSet.pressureAltitudeSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::PRESSURE, "ALTITUDE", "T|V", sampleSet.pressureAltitudeSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.pressureAltitudeSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.pressureAltitudeSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.pressureAltitudeSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Rain.
	// Start rain samples.
	sampleBaseTimestamp = sampleSet.rainSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::RAIN, "VALUES", "T|V", sampleSet.rainSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.rainSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.rainSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.rainSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Rain state.
	// Start rain state samples.
	sampleBaseTimestamp = sampleSet.rainStateSample.first();
	groupPacketStartMessage(Sensors::RAIN, "STATE", "T|V", 1, sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	sentMessages += sendSample(sampleSet.rainStateSample.first(), sampleSet.rainStateSample.second().c_str(), sampleBaseTimestamp); messageCount++;
	#pragma endregion

	#pragma region Temperature.
	// Start temperature samples.
	sampleBaseTimestamp = sampleSet.temperatureSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::TEMPERATURE, "VALUES", "T|V", sampleSet.temperatureSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.temperatureSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.temperatureSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.temperatureSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Humidity.
	// Start humidity samples.
	sampleBaseTimestamp = sampleSet.humiditySamples.peek(0)->first();
	groupPacketStartMessage(Sensors::HUMIDITY, "VALUES", "T|V", sampleSet.humiditySamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.humiditySamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.humiditySamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.humiditySamples.isEmpty());
	}
	#pragma endregion

	#pragma region Water temperature.
	// Start waterTemperature samples.
	sampleBaseTimestamp = sampleSet.waterTemperatureSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::WATER_TEMPERATURE, "VALUES", "T|V", sampleSet.waterTemperatureSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.waterTemperatureSamples.isEmpty()) {
		Pair<unsigned long, float> sample;
		sampleSet.waterTemperatureSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.waterTemperatureSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Wind direction.
	// Start wind direction samples.
	sampleBaseTimestamp = sampleSet.windDirectionSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::WIND_DIRECTION, "VALUES", "T|V", sampleSet.windDirectionSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.windDirectionSamples.isEmpty()) {
		Pair<unsigned long, int> sample;
		sampleSet.windDirectionSamples.pull(&sample);
		sendSampleCompact(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.windDirectionSamples.isEmpty());
	}
	#pragma endregion

	#pragma region Wind speed.
	// Start wind speed samples.
	sampleBaseTimestamp = sampleSet.windSpeedSamples.peek(0)->first();
	groupPacketStartMessage(Sensors::WIND_SPEED, "VALUES", "T|S|TS", sampleSet.windSpeedSamples.numElements(), sampleBaseTimestamp, formatBuff);
	sentMessages += sendMessage(RadioCommands::SAMPLE_DEVICE_GROUP_START, formatBuff); messageCount++;

	// Samples.
	while (!sampleSet.windSpeedSamples.isEmpty()) {
		Pair<unsigned long, windspeedpoint> sample;
		sampleSet.windSpeedSamples.pull(&sample);
		snprintf(formatBuff, sizeof(formatBuff), "%s|%s", String(sample.second().speedCorrected, 2).c_str(), String(sample.second().sensorTemperature, 2).c_str());
		sendSampleCompact(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, messageCount, sentMessages, sampleSet.windSpeedSamples.isEmpty());
	}
	#pragma endregion
	
	// Tell base station we are ending samples.
	sprintf(formatBuff, "T:%lu", millis());
	sentMessages += sendMessage(RadioCommands::SAMPLES_FINISH, formatBuff); messageCount++;

	// Put radio back to sleep to save power.
	sleep();

	return Pair<int, int>(sentMessages, messageCount);
}

