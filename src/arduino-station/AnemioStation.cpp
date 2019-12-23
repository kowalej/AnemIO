#include "AnemioStation.h"

AnemioStation::AnemioStation() {
	for (int i = 0; i < Devices::TOTAL; i++) {
		_online[i] = false;
		_lastCheck[i] = 0;
	}
	_radioLastTransmit = 0;
}

void AnemioStation::setup() {
	// Setup radio - if we cannot setup succesfully, we will retry a few times, and if we're still not setup, we will restart the Arduino.
	bool radioSetup = false;
	for (int i = 0; i < 3; i++) {
		radioSetup = _radioTransceiver.setup();
		if (radioSetup) {
			break;
		}
	}
	if (!radioSetup) {
		soft_restart();
	}

	// Startup i2c interface.
	Wire.begin();

	// Start using radio for a bit (wake up).
	_radioTransceiver.wake();

	_radioTransceiver.sendMessage(RadioCommands::SETUP_START, "Setup starting.");

	// Setup the providers (using 3 retries) and report status.
	int numOffline = setupProviders(3);

	debugA("Setup completed with %d device(s) offline.\n\n", numOffline);
	snprintf(formatBuff, sizeof(formatBuff), "N:%d - Setup completed with %d device(s) offline.", numOffline, numOffline);
	_radioTransceiver.sendMessage(RadioCommands::SETUP_FINISH, formatBuff);

	// Done with radio for now (sleep).
	_radioTransceiver.sleep();
}

bool AnemioStation::checkDeviceOnline_(Devices device) {
	switch (device) {
		case Devices::AMBIENT_LIGHT:
			return _ambientLightProvider.isOnline();
		case Devices::COMPASS_ACCELEROMETER:
			return _compassAccelerometerProvider.isOnline();
		case Devices::PRESSURE:
			return _pressureProvider.isOnline();
		case Devices::RAIN:
			return _rainProvider.isOnline();
		case Devices::TEMPERATURE_HUMIDITY:
			return _temperatureHumidityProvider.isOnline();
		case Devices::WATER_TEMPERATURE:
			return _waterTemperatureProvider.isOnline();
		case Devices::WIND_DIRECTION:
			return _windDirectionProvider.isOnline();
		case Devices::WIND_SPEED:
			return _windSpeedProvider.isOnline();
	}
}

bool AnemioStation::setupDevice_(Devices device) {
	switch (device) {
		case Devices::AMBIENT_LIGHT:
			return _ambientLightProvider.setup();
		case Devices::COMPASS_ACCELEROMETER:
			return _compassAccelerometerProvider.setup();
		case Devices::PRESSURE:
			return _pressureProvider.setup();
		case Devices::RAIN:
			return _rainProvider.setup();
		case Devices::TEMPERATURE_HUMIDITY:
			return _temperatureHumidityProvider.setup();
		case Devices::WATER_TEMPERATURE:
			return _waterTemperatureProvider.setup();
		case Devices::WIND_DIRECTION:
			return _windDirectionProvider.setup();
		case Devices::WIND_SPEED:
			return _windSpeedProvider.setup();
	}
}

int AnemioStation::setupProviders(int numberOfRetries) {
	int numberOffline;
	int retries = 0;

	do {
		numberOffline = 0;
		bool retry = retries < numberOfRetries - 1;
		String retryMsg = retry ? "will retry in a moment" : "will not retry";
		for (int i = 0; i < Devices::TOTAL; i++) {
			if (!_online[i]) {
				bool setup = setupDevice_(static_cast<Devices>(i));
				_online[i] = setup;
				if (setup) {
					debugA("%s setup was successful.", DeviceNames[i]);
					snprintf(formatBuff, sizeof(formatBuff), "D:%dO:%d - %s setup was successful.", i, setup, DeviceNames[i]);
					// Radio should already be ready during startup.
					_radioTransceiver.sendMessage(RadioCommands::REPORT_SETUP_STATE, formatBuff);
				}
				else {
					debugA("%s setup was unsuccessful %s.", DeviceNames[i], retryMsg);
					if (!retry) {
						snprintf(formatBuff, sizeof(formatBuff), "D:%dO:%d - %s setup was unsuccessfully.", i, setup, DeviceNames[i]);
						// Radio should already be ready during startup.
						_radioTransceiver.sendMessage(RadioCommands::REPORT_SETUP_STATE, formatBuff);
					}
				}
				numberOffline += setup ? 0 : 1;
			}
		}
		
		retries += 1;
	} while (numberOffline > 0 && retries < numberOfRetries);

	return numberOffline;
}

void AnemioStation::healthCheck() {
	for (int i = 0; i < Devices::TOTAL; i++) {
		bool isOnline = checkDeviceOnline_(static_cast<Devices>(i));
		// If state changed...
		if (_online[i] != isOnline) {
			_online[i] = isOnline;
			debugA("%s sensing unexpectedly %s.", DeviceNames[i], isOnline ? "came online" : "went offline");
			snprintf(formatBuff, sizeof(formatBuff), "D:%dO:%d - %s sensing unexpectedly %s", i, isOnline, DeviceNames[i], isOnline ? "came online" : "went offline");
			_radioTransceiver.sendMessageWithAutoWake(RadioCommands::REPORT_ONLINE_STATE, formatBuff);
		}
	}
}

void AnemioStation::loop() {
	// Check if each device is online.
	healthCheck();

	// Ambient Light.
	if (_online[Devices::AMBIENT_LIGHT] && (TIME_DELTA(_lastCheck[Devices::AMBIENT_LIGHT]) >= UPDATE_RATE_MS(AMBIENT_LIGHT_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Ambient light check start: %lu\n", millis());

		float ambientLightValue = _ambientLightProvider.getAmbientLightValue();
		_sampleSet.ambientLightSamples.add(Pair<unsigned long, float>(millis(), ambientLightValue), false);

		debugD("Ambient Light Sensor Values: %s", String(ambientLightValue).c_str());

		_lastCheck[Devices::AMBIENT_LIGHT] = millis();

		debugD("Ambient light check end: %lu\n", millis());
	}

	// Compass / Accelerometer.
	if (_online[Devices::COMPASS_ACCELEROMETER] && (TIME_DELTA(_lastCheck[Devices::COMPASS_ACCELEROMETER]) >= UPDATE_RATE_MS(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Compass accelerometer check start: %lu\n", millis());

		coord compassXYZ = _compassAccelerometerProvider.getCompass();
		_sampleSet.compassXYZSamples.add(Pair<unsigned long, coord>(millis(), compassXYZ), false);

		int compassHeading = _compassAccelerometerProvider.getHeading(compassXYZ.x, compassXYZ.y);
		_sampleSet.compassHeadingSamples.add(Pair<unsigned long, int>(millis(), compassHeading), false);

		coord accelerometerXYZ = _compassAccelerometerProvider.getAccelerometer();
		_sampleSet.accelerometerXYZSamples.add(Pair<unsigned long, coord>(millis(), accelerometerXYZ), false);

		debugD("Compass / Accelerometer Sensor Values:");
		debugD("  Compass (X,Y,Z) %s, %s, %s", String(compassXYZ.x).c_str(), String(compassXYZ.y).c_str(), String(compassXYZ.z).c_str());
		debugD("  Compass Heading %s", String(compassHeading).c_str());
		debugD("  Accelerometer (X,Y,Z) %s %s %s", String(accelerometerXYZ.x).c_str(), String(accelerometerXYZ.y).c_str(), String(accelerometerXYZ.z).c_str());

		_lastCheck[Devices::COMPASS_ACCELEROMETER] = millis();

		debugD("Compass accelerometer check end: %lu\n", millis());
	}

	// Pressure (+ Backup Temperature and Altitude).
	if (_online[Devices::PRESSURE] && (TIME_DELTA(_lastCheck[Devices::PRESSURE]) >= UPDATE_RATE_MS(PRESSURE_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Pressure check start: %lu\n", millis());

		float pressureValue = _pressureProvider.getPressure();
		_sampleSet.pressureSamples.add(Pair<unsigned long, float>(millis(), pressureValue), false);

		float pressureTemperatureValue = _pressureProvider.getTemperature();
		_sampleSet.pressureTemperatureSamples.add(Pair<unsigned long, float>(millis(), pressureTemperatureValue), false);

		float pressureAltitudeValue = _pressureProvider.getAltitude();
		_sampleSet.pressureAltitudeSamples.add(Pair<unsigned long, float>(millis(), pressureAltitudeValue), false);

		debugD("Pressure Sensor Values:");
		debugD("  Pressure (Pascals) %s", String(pressureValue).c_str());
		debugD("  Temperature (Celcius) %s", String(pressureTemperatureValue).c_str());
		debugD("  Altitude Estimation (Meters) %s", String(pressureAltitudeValue).c_str());

		_lastCheck[Devices::PRESSURE] = millis();

		debugD("Pressure check end: %lu", millis());
	}

	// Rain.
	if (_online[Devices::RAIN] && (TIME_DELTA(_lastCheck[Devices::RAIN]) >= UPDATE_RATE_MS(RAIN_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Rain check start: %lu\n", millis());

		float rainValue = _rainProvider.getRainValue();
		_sampleSet.rainSamples.add(Pair<unsigned long, float>(millis(), rainValue), false);

		debugD("Rain Sensor Values: %s", String(rainValue).c_str());

		_lastCheck[Devices::RAIN] = millis();

		debugD("Rain check end: %lu\n", millis());
	}

	// Temperature / Humidity.
	if (_online[Devices::TEMPERATURE_HUMIDITY] && (TIME_DELTA(_lastCheck[Devices::TEMPERATURE_HUMIDITY]) >= UPDATE_RATE_MS(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Temperature / humidity check start: %lu\n", millis());

		float temperatureValue = _temperatureHumidityProvider.getTemperature();
		_sampleSet.temperatureSamples.add(Pair<unsigned long, float>(millis(), temperatureValue), false);

		float humidityValue = _temperatureHumidityProvider.getHumidity();
		_sampleSet.humiditySamples.add(Pair<unsigned long, float>(millis(), humidityValue), false);

		debugD("Temperature / Humidity Sensor Values:");
		debugD("  Temperature (Celcius) %s", String(temperatureValue).c_str());
		debugD("  Humidity (%%) %s", String(humidityValue).c_str());

		_lastCheck[Devices::TEMPERATURE_HUMIDITY] = millis();

		debugD("Temperature / humidity check end: %lu\n", millis());
	}

	// Water Temperature.
	if (_online[Devices::WATER_TEMPERATURE] && (TIME_DELTA(_lastCheck[Devices::WATER_TEMPERATURE]) >= UPDATE_RATE_MS(WATER_TEMP_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Water temperature check start: %lu\n", millis());

		float waterTemperature = _waterTemperatureProvider.getWaterTemperature();
		_sampleSet.waterTemperatureSamples.add(Pair<unsigned long, float>(millis(), waterTemperature), false);

		debugD("Water Temperature Sensor Values: %s", String(waterTemperature).c_str());

		_lastCheck[Devices::WATER_TEMPERATURE] = millis();

		debugD("Water temperature check end: %lu\n", millis());
	}

	// Wind Direction.
	if (_online[Devices::WIND_DIRECTION] && (TIME_DELTA(_lastCheck[Devices::WIND_DIRECTION]) >= UPDATE_RATE_MS(WIND_DIRECTION_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Wind direction check start: %lu\n", millis());

		int windHeadingRaw = _windDirectionProvider.getHeading();
		int windHeadingCorrected = NAN;
		
		// Get the compass corrected wind direction (if possible).
		if (_compassAccelerometerProvider.isOnline()) {
			coord compassHeading = _compassAccelerometerProvider.getCompass();
			 windHeadingCorrected = _windDirectionProvider.getCorrectedHeading(
				windHeadingRaw,
				_compassAccelerometerProvider.getHeading(compassHeading.x, compassHeading.y)
			);
		}
		_sampleSet.windDirectionSamples.add(Pair<unsigned long, int>(millis(), windHeadingCorrected), false);
		debugD("Wind Direction Sensor Values:");
		debugD("  Wind Heading Raw %d", windHeadingRaw);
		debugD("  Wind Heading Corrected %d", windHeadingCorrected);

		_lastCheck[Devices::WIND_DIRECTION] = millis();

		debugD("Wind direction check end: %lu\n", millis());
	}

	// Wind Speed.
	if (_online[Devices::WIND_SPEED] && (TIME_DELTA(_lastCheck[Devices::WIND_SPEED]) >= UPDATE_RATE_MS(WIND_SPEED_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Wind speed / temperature check start: %lu\n", millis());

		float windSpeedRaw = _windSpeedProvider.getWindSpeedRaw();

		// Wind ambient temperature.
		float windSensorTemperature = _windSpeedProvider.getWindSensorTemperature();

		// Temperature corrected wind speed. We will send this with the temperature.
		// We can always reverse the math at the ground station if we don't like the corrected values.
		float windSpeedCorrected = _windSpeedProvider.getCorrectedWindSpeed(windSensorTemperature);
		_sampleSet.windSpeedSamples.add(Pair<unsigned long, windspeedpoint>(millis(), windspeedpoint(windSpeedCorrected, windSensorTemperature)), false);

		debugD("Wind Speed Sensor Values:");
		debugD("  Wind Speed Raw (Knots) %s", String(windSpeedRaw).c_str());
		debugD("  Wind Sensor Temperature (Celcius) %s", String(windSensorTemperature).c_str());
		debugD("  Wind Speed Temperature Corrected %s", String(windSpeedCorrected).c_str());

		_lastCheck[Devices::WIND_SPEED] = millis();

		debugD("Wind speed / temperature check end: %lu\n", millis());
	}

	// Transmit data to "ground" station.
	if (TIME_DELTA(_radioLastTransmit) >= RADIO_SEND_INTERVAL_MS) {

		// Aggregation for rain state.
		String rainState = _rainProvider.getRainState(_sampleSet);
		debugD("Rain state: %s", rainState.c_str());
		_sampleSet.rainStateSample = Pair<unsigned long, String>(millis(), rainState);

		// Aggregation for ambient light state.
		String ambientLightState = _ambientLightProvider.getAmbientLightState(_sampleSet);
		debugD("Ambient light state: %s", ambientLightState.c_str());
		_sampleSet.ambientLightStateSample = Pair<unsigned long, String>(millis(), ambientLightState);

		// Send data over radio.
		debugD("Radio transmit start: %lu\n", millis());
		Pair<int,int> sentResults = _radioTransceiver.sendSamples(_sampleSet);
		debugD("Radio sent %d/%d messages. Percent success: %f.", sentResults.first(), sentResults.second(), (static_cast<double>(sentResults.first()) / static_cast<double>(sentResults.second())) );
		_radioLastTransmit = millis();
		debugD("Radio transmit end: %lu\n", _radioLastTransmit);
	}
}
