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
	debugA("Radio initialized? %s.", radioInit ? "true" : "false");

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

String RadioTransceiver::receive(int wait) {
	wake();
	char message[61];
	unsigned long startTime = millis();
	while(millis() - startTime < wait)
	{
		if (_radio.receiveDone()) {
			for (byte i = 0; i < _radio.DATALEN; i++) {
				message[i] = (char)_radio.DATA[i];
			}
			sleep();
			return message;
		}
		delay(10);
	}
	sleep();
	return "";
}

bool RadioTransceiver::sendMessageWithAutoWake(int command, const char* message, uint8_t retries) {
	wake();
	bool sent = sendMessage(command, message, retries);
	sleep();
	return sent;
}

bool RadioTransceiver::sendMessage(int command, const char* message, uint8_t retries) {
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH] = { '\0' };
	snprintf(formatBuff, sizeof(formatBuff), "[%d]%s", command, message);
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), retries, RADIO_RETRY_WAIT_MS);
	return sent;
}

inline unsigned long getRelativeTimestamp(unsigned long timestamp, unsigned long baseTimestamp) {
	return timestamp - baseTimestamp;
}

inline double calcroudtripAverage(double current, double newVal) {
	if (current == -1) return newVal;
	else return (current + newVal) / 2;
}

// Sends a single sample.
bool RadioTransceiver::sendSample(unsigned long timestamp, const char* serializedValue, unsigned long baseTimestamp) {
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH];
	// Format into [<command><timestamp>],<value>.
	snprintf(formatBuff, sizeof(formatBuff), "[%d]%lu,%s", RadioCommands::SAMPLE_WRITE, getRelativeTimestamp(timestamp, baseTimestamp), serializedValue);
	bool sent = _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS);
	return sent;
}

// Only sends a message when the radio buffer is full.
void RadioTransceiver::sendMessageCompact(const char* message, char* messageBuff, int& numSent, int& numSuccess, double& roudtripAverage, bool flush) {
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH];
	unsigned long messageSendTime;
	int currentBuffLength = strlen(messageBuff);
	int newMessageLength = snprintf(formatBuff, sizeof(formatBuff), "%s%s", currentBuffLength < 1 ? COMPACT_MESSAGES_START : "", message);
	strncat(messageBuff, formatBuff, newMessageLength);
	int messagesLength = currentBuffLength + newMessageLength;
	if (messagesLength >= RADIO_MAX_MESSAGE_LENGTH) {
		messageSendTime = millis();
		numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, messageBuff, RADIO_MAX_MESSAGE_LENGTH, RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
		roudtripAverage = calcroudtripAverage(roudtripAverage, millis() - messageSendTime);
		int charsRemaining = messagesLength - RADIO_MAX_MESSAGE_LENGTH;
		for (int i = 0; i < charsRemaining; i++) {
			messageBuff[i] = messageBuff[RADIO_MAX_MESSAGE_LENGTH + i];
		}
		messageBuff[charsRemaining] = null;
	}

	if (flush) {
		strncat(messageBuff, COMPACT_MESSAGES_END, strlen(COMPACT_MESSAGES_END));
		int messagesLength = strlen(messageBuff);
		int buffInd = 0;

		// Clean formatBuff.
		for (int i = 0; i < sizeof(formatBuff); i++) {
			formatBuff[i] = null;
		}

		for (int i = 0; i < messagesLength; i++) {
			formatBuff[buffInd] = messageBuff[i];
			buffInd++;
			if (buffInd == RADIO_MAX_MESSAGE_LENGTH - 1 || i == messagesLength - 1) {
				messageSendTime = millis();
				numSuccess += _radio.sendWithRetry(RADIO_BASE_NODE_ID, formatBuff, strlen(formatBuff), RADIO_RETRY_NUM, RADIO_RETRY_WAIT_MS); numSent++;
				roudtripAverage = calcroudtripAverage(roudtripAverage, millis() - messageSendTime);
				buffInd = 0;
				for (int i = 0; i < sizeof(formatBuff); i++) {
					formatBuff[i] = null;
				}
			}
		}
		messageBuff[0] = null;
	}
}

void sampleMessage(unsigned long timestamp, const char* serializedValues, unsigned long baseTimestamp, char* formatBuff, bool last) {
	sprintf(formatBuff, "%lu,%s%s", getRelativeTimestamp(timestamp, baseTimestamp), serializedValues, !last ? "|" : "");
}

void sampleGroupDividerMessage(Readings::Readings sensorCategory, const char* headerFormat, size_t numElements, unsigned long sampleBaseTimestamp, unsigned long samplesStartTime, char* formatBuff, double roudtripAverage) {
	// S: Sensor reading category.
	// F: Header format (labels).
	// N: Number of elements in upcoming series of samples.
	// B: Relative (implicitly negative) value of first timestamp in the group, compared to the samples start time.
	// R: Roundtrip average.
	sprintf(formatBuff, "[%d]S:%dF:%sN:%uB:%luR:%s[7]", RadioCommands::SAMPLE_GROUP_DIVIDER, sensorCategory, headerFormat, numElements, getRelativeTimestamp(samplesStartTime, sampleBaseTimestamp), String(roudtripAverage, 2).c_str());
}

Pair<int, int> RadioTransceiver::sendSamples(SampleSet& sampleSet) {
	int numSent = 0;
	int numSuccess = 0;
	double roudtripAverage = -1;
	unsigned long sampleBaseTimestamp;
	char formatBuff[RADIO_MAX_MESSAGE_LENGTH] = { null };
	char sampleBuff[RADIO_MAX_MESSAGE_LENGTH] = { null };
	char messageBuff[RADIO_MAX_MESSAGE_LENGTH * 2] = { null };

	// Wake up the radio before sending this series of messages.
	wake();

	// Tell base station we are starting samples.
	unsigned long samplesStartTime = millis();
	sprintf(formatBuff, "[%d]T:%lu", RadioCommands::SAMPLES_START, samplesStartTime);
	sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

#pragma region Accelerometer XYZ.
	if (!sampleSet.accelerometerXYZSamples.isEmpty()) {
		// Start accelerometer XYZ samples.
		sampleBaseTimestamp = sampleSet.accelerometerXYZSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::ACCELEROMETER_XYZ, "T,X,Y,Z", sampleSet.accelerometerXYZSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.accelerometerXYZSamples.isEmpty()) {
			Pair<unsigned long, coord> sample;
			sampleSet.accelerometerXYZSamples.pull(&sample);
			snprintf(formatBuff, sizeof(formatBuff), "%s,%s,%s", String(sample.second().x, 2).c_str(), String(sample.second().y, 2).c_str(), String(sample.second().z, 2).c_str());
			sampleMessage(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, sampleSet.accelerometerXYZSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Ambient Light Values.
	if (!sampleSet.ambientLightSamples.isEmpty()) {
		// Start ambient light samples.
		sampleBaseTimestamp = sampleSet.ambientLightSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::AMBIENT_LIGHT_VALUES, "T,V", sampleSet.ambientLightSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.ambientLightSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.ambientLightSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.ambientLightSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Ambient Light State
	if (!sampleSet.ambientLightStateSamples.isEmpty()) {
		// Start ambient light state samples.
		sampleBaseTimestamp = sampleSet.ambientLightStateSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::AMBIENT_LIGHT_STATE, "T,V", sampleSet.ambientLightStateSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.ambientLightStateSamples.isEmpty()) {
			Pair<unsigned long, String> sample;
			sampleSet.ambientLightStateSamples.pull(&sample);
			sampleMessage(sample.first(), sample.second().c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.ambientLightStateSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Battery Level
	if (!sampleSet.ambientLightStateSamples.isEmpty()) {
		// Start battery levelsamples.
		sampleBaseTimestamp = sampleSet.batteryLevelSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::BATTERY_LEVEL, "T,V", sampleSet.batteryLevelSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.batteryLevelSamples.isEmpty()) {
			Pair<unsigned long, String> sample;
			sampleSet.batteryLevelSamples.pull(&sample);
			sampleMessage(sample.first(), sample.second().c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.batteryLevelSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Compass XYZ.
	if (!sampleSet.compassXYZSamples.isEmpty()) {
		// Start compass XYZ samples.
		sampleBaseTimestamp = sampleSet.compassXYZSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::COMPASS_XYZ, "T,X,Y,Z", sampleSet.compassXYZSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.compassXYZSamples.isEmpty()) {
			Pair<unsigned long, coord> sample;
			sampleSet.compassXYZSamples.pull(&sample);
			snprintf(formatBuff, sizeof(formatBuff), "%s,%s,%s", String(sample.second().x, 2).c_str(), String(sample.second().y, 2).c_str(), String(sample.second().z, 2).c_str());
			sampleMessage(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, sampleSet.compassXYZSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Compass Heading.
	if (!sampleSet.compassHeadingSamples.isEmpty()) {
		// Start compass heading samples.
		sampleBaseTimestamp = sampleSet.compassHeadingSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::COMPASS_HEADING, "T,V", sampleSet.compassHeadingSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.compassHeadingSamples.isEmpty()) {
			Pair<unsigned long, int> sample;
			sampleSet.compassHeadingSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.compassHeadingSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Pressure.
	if (!sampleSet.pressureSamples.isEmpty()) {
		// Start pressure samples.
		sampleBaseTimestamp = sampleSet.pressureSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::PRESSURE_PRESSURE, "T,V", sampleSet.pressureSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.pressureSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.pressureSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.pressureSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Pressure sensor temperature.
	if (!sampleSet.pressureTemperatureSamples.isEmpty()) {
		// Start pressure sensor temperature samples.
		sampleBaseTimestamp = sampleSet.pressureTemperatureSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::PRESSURE_TEMPERATURE, "T,V", sampleSet.pressureTemperatureSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.pressureTemperatureSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.pressureTemperatureSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.pressureTemperatureSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Pressure altitude estimate.
	if (!sampleSet.pressureAltitudeSamples.isEmpty()) {
		// Start pressure altitude estimate samples.
		sampleBaseTimestamp = sampleSet.pressureAltitudeSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::PRESSURE_ALTITUDE, "T,V", sampleSet.pressureAltitudeSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.pressureAltitudeSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.pressureAltitudeSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.pressureAltitudeSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Rain.
	if (!sampleSet.rainSamples.isEmpty()) {
		// Start rain samples.
		sampleBaseTimestamp = sampleSet.rainSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::RAIN_VALUES, "T,V", sampleSet.rainSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.rainSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.rainSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.rainSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Rain state.
	if (!sampleSet.rainStateSamples.isEmpty()) {
		// Start rain state samples.
		sampleBaseTimestamp = sampleSet.rainStateSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::RAIN_STATE, "T,V", sampleSet.rainStateSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.rainStateSamples.isEmpty()) {
			Pair<unsigned long, String> sample;
			sampleSet.rainStateSamples.pull(&sample);
			sampleMessage(sample.first(), sample.second().c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.rainStateSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Temperature.
	if (!sampleSet.temperatureSamples.isEmpty()) {
		// Start temperature samples.
		sampleBaseTimestamp = sampleSet.temperatureSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::TEMPERATURE, "T,V", sampleSet.temperatureSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.temperatureSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.temperatureSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.temperatureSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Humidity.
	if (!sampleSet.humiditySamples.isEmpty()) {
		// Start humidity samples.
		sampleBaseTimestamp = sampleSet.humiditySamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::HUMIDITY, "T,V", sampleSet.humiditySamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.humiditySamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.humiditySamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.humiditySamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Water temperature.
	if (!sampleSet.waterTemperatureSamples.isEmpty()) {
		// Start waterTemperature samples.
		sampleBaseTimestamp = sampleSet.waterTemperatureSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::WATER_TEMPERATURE, "T,V", sampleSet.waterTemperatureSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.waterTemperatureSamples.isEmpty()) {
			Pair<unsigned long, float> sample;
			sampleSet.waterTemperatureSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second(), 2).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.waterTemperatureSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Wind direction.
	if (!sampleSet.windDirectionSamples.isEmpty()) {
		// Start wind direction samples.
		sampleBaseTimestamp = sampleSet.windDirectionSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::WIND_DIRECTION, "T,V", sampleSet.windDirectionSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.windDirectionSamples.isEmpty()) {
			Pair<unsigned long, int> sample;
			sampleSet.windDirectionSamples.pull(&sample);
			sampleMessage(sample.first(), String(sample.second()).c_str(), sampleBaseTimestamp, sampleBuff, sampleSet.windDirectionSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

#pragma region Wind speed.
	if (!sampleSet.windSpeedSamples.isEmpty()) {
		// Start wind speed samples.
		sampleBaseTimestamp = sampleSet.windSpeedSamples.peek(0)->first();
		sampleGroupDividerMessage(Readings::Readings::WIND_SPEED, "T,V,TC", sampleSet.windSpeedSamples.numElements(), sampleBaseTimestamp, samplesStartTime, formatBuff, roudtripAverage);
		sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage);

		// Samples.
		while (!sampleSet.windSpeedSamples.isEmpty()) {
			Pair<unsigned long, windspeedpoint> sample;
			sampleSet.windSpeedSamples.pull(&sample);
			snprintf(formatBuff, sizeof(formatBuff), "%s,%s", String(sample.second().speedCorrected, 2).c_str(), String(sample.second().sensorTemperature, 2).c_str());
			sampleMessage(sample.first(), formatBuff, sampleBaseTimestamp, sampleBuff, sampleSet.windSpeedSamples.isEmpty());
			sendMessageCompact(sampleBuff, messageBuff, numSent, numSuccess, roudtripAverage);
		}
	}
#pragma endregion

	// Tell base station we are ending samples and how many messages were sent successfully out of total.
	snprintf(formatBuff, sizeof(formatBuff), "[%d]S:%d/%dT:%luR:%s", RadioCommands::SAMPLES_FINISH, (numSuccess + 1), (numSent + 1), millis(), String(roudtripAverage, 2).c_str());
	sendMessageCompact(formatBuff, messageBuff, numSent, numSuccess, roudtripAverage, true);

	// Put radio back to sleep to save power.
	sleep();

	return Pair<int, int>(numSuccess, numSent);
}
