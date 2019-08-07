#include "AnemioStation.h"

AnemioStation::AnemioStation() {}

void AnemioStation::setup() {
	for (int i = 0; i < Devices::TOTAL; i++) {
		_online[i] = false;
		_lastCheck[i] = 0;
	}

	// Startup i2c interface.
	Wire.begin();

	// TODO: check setup completes and return info to radio.
	
	// Setup the providers and report status.
	int offline = setupProviders(3);
	debugA("Setup complete with %d device(s) offline.\n\n", offline);
}

int AnemioStation::setupProviders(int numberOfRetries) {
	int numberOffline;
	int retries = 0;

	do {
		numberOffline = 0;
		String retryMsg = retries < numberOfRetries - 1 ? "will retry in a moment" : "will not retry";

		// Setup pressure sensor.
		if (!_online[Devices::PRESSURE]) {
			if ((_online[Devices::PRESSURE] = _pressureProvider.setup())) {
				debugA("Pressure sensing now online.");
			}
			else {
				debugA("Problem bringing pressure sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::PRESSURE] ? 0 : 1;

		// Setup rain sensor.
		if (!_online[Devices::RAIN]) {
			if ((_online[Devices::RAIN] = _rainProvider.setup())) {
				debugA("Rain sensing now online.");
			}
			else {
				debugA("Problem bringing rain sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::RAIN] ? 0 : 1;

		// Setup temperature / humidity sensor.
		if (!_online[Devices::TEMPERATURE_HUMIDITY]) {
			if ((_online[Devices::TEMPERATURE_HUMIDITY] = _temperatureHumidityProvider.setup())) {
				debugA("Temperature / humidity sensing now online.");
			}
			else {
				debugA("Problem bringing temperature / humidity sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::TEMPERATURE_HUMIDITY] ? 0 : 1;

		// Setup ambient light sensor.
		if (!_online[Devices::AMBIENT_LIGHT]) {
			if ((_online[Devices::AMBIENT_LIGHT] = _ambientLightProvider.setup())) {
				debugA("Ambient light sensing now online.");
			}
			else {
				debugA("Problem bringing ambient light sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::AMBIENT_LIGHT] ? 0 : 1;

		// Setup wind sensor.
		if (!_online[Devices::WIND_SPEED]) {
			if ((_online[Devices::WIND_SPEED] = _windSpeedProvider.setup())) {
				debugA("Wind speed sensing now online.");
			}
			else {
				debugA("Problem bringing wind speed sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::WIND_SPEED] ? 0 : 1;

		// Setup compass / accelerometer.
		if (!_online[Devices::COMPASS_ACCELEROMETER]) {
			if ((_online[Devices::COMPASS_ACCELEROMETER] = _compassAccelerometerProvider.setup())) {
				debugA("Compass / accelerometer sensing now online.");
			}
			else {
				debugA("Problem bringing compass / accelerometer sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::COMPASS_ACCELEROMETER] ? 0 : 1;

		retries += 1;
	} while (numberOffline > 0 && retries < numberOfRetries);

	return numberOffline;
}

int AnemioStation::healthCheck() {
	int numberOffline;

	// Check pressure is online.
	if (!(_online[Devices::PRESSURE] = _pressureProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check rain is online.
	if (!(_online[Devices::RAIN] = _rainProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check temperature / humidity is online.
	if (!(_online[Devices::TEMPERATURE_HUMIDITY] = _temperatureHumidityProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check ambient light is online.
	if (!(_online[Devices::AMBIENT_LIGHT] = _ambientLightProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check wind speed is online.
	if (!(_online[Devices::WIND_SPEED] = _windSpeedProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check compass / accelerometer is online.
	if (!(_online[Devices::COMPASS_ACCELEROMETER] = _compassAccelerometerProvider.isOnline())) {
		numberOffline += 1;
	}

	return numberOffline; // Should hopefully never resolve to more than 0.
}

void AnemioStation::loop() {
	// Check if each device is online.
	healthCheck();

	//// Pressure (Temperature) and Altitude.
	//if (_online[Devices::PRESSURE] && (millis() - _lastCheck[Devices::PRESSURE] > UPDATE_RATE_MS(PRESSURE_UPDATE_RATE_HZ))) {
	//	debugD("----------------------");
	//	debugD("Pressure check start: %lu\n", millis());

	//	float pressureValue = _pressureProvider.getPressure();
	//	_sampleSet.pressureSamples.add(Pair<int, float>(millis(), pressureValue), true);

	//	float pressureTemperatureValue = _pressureProvider.getTemperature();
	//	_sampleSet.pressureTemperatureSamples.add(Pair<int, float>(millis(), pressureTemperatureValue), false);

	//	float pressureAltitudeValue = _pressureProvider.getAltitude();
	//	_sampleSet.pressureAltitudeSamples.add(Pair<int, float>(millis(), pressureAltitudeValue), false);

	//	debugD("Pressure Sensor Values:");
	//	debugD("  Pressure (Pascals) %s", String(pressureValue).c_str());
	//	debugD("  Temperature (Celcius) %s", String(pressureTemperatureValue).c_str());
	//	debugD("  Altitude Estimation (Meters) %s", String(pressureAltitudeValue).c_str());

	//	_lastCheck[Devices::PRESSURE] = millis();

	//	debugD("Pressure check end: %lu", millis());
	//}

	//// Rain.
	//if (_online[Devices::RAIN] && (millis() - _lastCheck[Devices::RAIN] > UPDATE_RATE_MS(RAIN_UPDATE_RATE_HZ))) {
	//	debugD("----------------------");
	//	debugD("Rain check start: %lu\n", millis());

	//	float rainValue = _rainProvider.getRainValue();
	//	_sampleSet.rainSamples.add(Pair<int, float>(millis(), rainValue), true);
	//	debugD("Rain Sensor Values: %s", String(rainValue).c_str());

	//	_lastCheck[Devices::RAIN] = millis();

	//	debugD("Rain check end: %lu\n", millis());
	//}

	//// Temperature / Humidity.
	//if (_online[Devices::TEMPERATURE_HUMIDITY] && (millis() - _lastCheck[Devices::TEMPERATURE_HUMIDITY] > UPDATE_RATE_MS(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ))) {
	//	debugD("----------------------");
	//	debugD("Temperature / humidity check start: %lu\n", millis());

	//	float tempValue = _temperatureHumidityProvider.getTemperature();
	//	_sampleSet.temperatureSamples.add(Pair<int, float>(millis(), tempValue), true);

	//	float humidityValue = _temperatureHumidityProvider.getHumidity();
	//	_sampleSet.humiditySamples.add(Pair<int, float>(millis(), humidityValue), true);

	//	debugD("Temperature / Humidity Sensor Values:");
	//	debugD("  Temperature (Celcius) %s", String(tempValue).c_str());
	//	debugD("  Humidity (%%) %s", String(humidityValue).c_str());

	//	_lastCheck[Devices::TEMPERATURE_HUMIDITY] = millis();

	//	debugD("Temperature / humidity check end: %lu\n", millis());
	//}

	//// Ambient Light.
	//if (_online[Devices::AMBIENT_LIGHT] && (millis() - _lastCheck[Devices::AMBIENT_LIGHT] > UPDATE_RATE_MS(AMBIENT_LIGHT_UPDATE_RATE_HZ))) {
	//	debugD("----------------------");
	//	debugD("Ambient light check start: %lu\n", millis());

	//	float ambientLightValue = _ambientLightProvider.getAmbientLightValue();
	//	_sampleSet.ambientLightSamples.add(Pair<int, float>(millis(), ambientLightValue), true);

	//	debugD("Ambient Light Sensor Values: %s", String(ambientLightValue).c_str());

	//	_lastCheck[Devices::AMBIENT_LIGHT] = millis();

	//	debugD("Ambient light check end: %lu\n", millis());
	//}

	// Wind Speed / Temperature.
	if (_online[Devices::WIND_SPEED] && (millis() - _lastCheck[Devices::WIND_SPEED] > UPDATE_RATE_MS(WIND_SPEED_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Wind speed / temperature check start: %lu\n", millis());

		float windSpeedRaw = _windSpeedProvider.getWindSpeedRaw();
		_sampleSet.windSpeedSamples.add(Pair<int, float>(millis(), windSpeedRaw), true);

		float windTemperature = _windSpeedProvider.getWindTemperature();
		//_sampleSet.windSpeedTemperatureSamples.add(Pair<int, float>(millis(), windSpeedTemperature), true);

		float windSpeedCorrected = _windSpeedProvider.getCorrectedWindSpeed(windTemperature);

		debugD("Wind Speed Sensor Values:");
		debugD("  Wind Speed Raw (Knots) %s", String(windSpeedRaw).c_str());
		debugD("  Wind Temperature (Celcius) %s", String(windTemperature).c_str());
		debugD("  Wind Speed Corrected %s", String(windSpeedCorrected).c_str());

		_lastCheck[Devices::WIND_SPEED] = millis();

		debugD("Wind speed / temperature check end: %lu\n", millis());
	}

	// Compass / Accelerometer.
	if (_online[Devices::COMPASS_ACCELEROMETER] && (millis() - _lastCheck[Devices::COMPASS_ACCELEROMETER] > UPDATE_RATE_MS(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Compass accelerometer check start: %lu\n", millis());

		//_sampleSet.windSpeedSamples.add(Pair<int, float>(millis(), windSpeedRaw), true);

		float* compassXYZ = _compassAccelerometerProvider.getCompass();
		int compassHeading = _compassAccelerometerProvider.getHeading(compassXYZ[0], compassXYZ[1]);
		float* accelerometerXYZ = _compassAccelerometerProvider.getAccelerometer();

		debugD("Compass / Accelerometer Sensor Values:");
		debugD("  Compass (X,Y,Z) %s, %s, %s", String(compassXYZ[0]).c_str(), String(compassXYZ[1]).c_str(), String(compassXYZ[2]).c_str());
		debugD("  Compass Heading %s", String(compassHeading).c_str());
		debugD("  Accelerometer (X,Y,Z) %s %s %s", String(accelerometerXYZ[0]).c_str(), String(accelerometerXYZ[1]).c_str(), String(accelerometerXYZ[2]).c_str());

		_lastCheck[Devices::COMPASS_ACCELEROMETER] = millis();

		debugD("Compass accelerometer check end: %lu\n", millis());
	}

	// Restart the station...
	///soft_restart();
}
