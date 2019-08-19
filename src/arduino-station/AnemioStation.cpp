#include "AnemioStation.h"

AnemioStation::AnemioStation() {
	for (int i = 0; i < Devices::TOTAL; i++) {
		_online[i] = false;
		_lastCheck[i] = 0;
	}
	_radioLastTransmit = 0;
	_screenLastUpdate = 0;
	_screenOn = true;//  TODO REMOVE.
}

void AnemioStation::setup() {
	// Startup i2c interface.
	Wire.begin();

	// Setup the providers and report status.
	int offline = setupProviders(3);

	// TODO: check setup completes with zero devices offline, and return info to radio.

	debugA("Setup complete with %d device(s) offline.\n\n", offline);

	/*lcd.begin(16, 2);
	lcd.print("Anemio Station Online");*/
}

int AnemioStation::setupProviders(int numberOfRetries) {
	int numberOffline;
	int retries = 0;

	do {
		numberOffline = 0;
		String retryMsg = retries < numberOfRetries - 1 ? "will retry in a moment" : "will not retry";

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

		// Setup water temperature.
		if (!_online[Devices::WATER_TEMPERATURE]) {
			if ((_online[Devices::WATER_TEMPERATURE] = _waterTemperatureProvider.setup())) {
				debugA("Water temperature sensing now online.");
			}
			else {
				debugA("Problem bringing water temperature sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::WATER_TEMPERATURE] ? 0 : 1;

		// Setup wind direction sensor.
		if (!_online[Devices::WIND_DIRECTION]) {
			if ((_online[Devices::WIND_DIRECTION] = _windDirectionProvider.setup())) {
				debugA("Wind direction sensing now online.");
			}
			else {
				debugA("Problem bringing wind direction sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::WIND_DIRECTION] ? 0 : 1;

		// Setup wind speed sensor.
		if (!_online[Devices::WIND_SPEED]) {
			if ((_online[Devices::WIND_SPEED] = _windSpeedProvider.setup())) {
				debugA("Wind speed sensing now online.");
			}
			else {
				debugA("Problem bringing wind speed sensing online, %s.", retryMsg.c_str());
			}
		}
		numberOffline += _online[Devices::WIND_SPEED] ? 0 : 1;

		retries += 1;
	} while (numberOffline > 0 && retries < numberOfRetries);

	return numberOffline;
}

int AnemioStation::healthCheck() {
	int numberOffline;

	// Check ambient light is online.
	if (!(_online[Devices::AMBIENT_LIGHT] = _ambientLightProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check compass / accelerometer is online.
	if (!(_online[Devices::COMPASS_ACCELEROMETER] = _compassAccelerometerProvider.isOnline())) {
		numberOffline += 1;
	}

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

	// Check water temperature is online.
	if (!(_online[Devices::WATER_TEMPERATURE] = _waterTemperatureProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check wind direction is online.
	if (!(_online[Devices::WIND_DIRECTION] = _windDirectionProvider.isOnline())) {
		numberOffline += 1;
	}

	// Check wind speed is online.
	if (!(_online[Devices::WIND_SPEED] = _windSpeedProvider.isOnline())) {
		numberOffline += 1;
	}

	return numberOffline; // Should hopefully never resolve to more than 0.
}

void AnemioStation::loop() {
	// Check if each device is online.
	healthCheck();

	// Ambient Light.
	if (_online[Devices::AMBIENT_LIGHT] && (TIME_DELTA(_lastCheck[Devices::AMBIENT_LIGHT]) >= UPDATE_RATE_MS(AMBIENT_LIGHT_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Ambient light check start: %lu\n", millis());

		float ambientLightValue = _ambientLightProvider.getAmbientLightValue();
		_sampleSet.ambientLightSamples.add(Pair<int, float>(millis(), ambientLightValue), false);

		debugD("Ambient Light Sensor Values: %s", String(ambientLightValue).c_str());

		_lastCheck[Devices::AMBIENT_LIGHT] = millis();

		debugD("Ambient light check end: %lu\n", millis());
	}

	// Compass / Accelerometer.
	if (_online[Devices::COMPASS_ACCELEROMETER] && (TIME_DELTA(_lastCheck[Devices::COMPASS_ACCELEROMETER]) >= UPDATE_RATE_MS(COMPASS_ACCELEROMETER_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Compass accelerometer check start: %lu\n", millis());

		coord compassXYZ = _compassAccelerometerProvider.getCompass();
		_sampleSet.compassXYZ.add(Pair<int, coord>(millis(), compassXYZ), false);

		int compassHeading = _compassAccelerometerProvider.getHeading(compassXYZ.x, compassXYZ.y);
		_sampleSet.compassHeading.add(Pair<int, int>(millis(), compassHeading), false);

		coord accelerometerXYZ = _compassAccelerometerProvider.getAccelerometer();
		_sampleSet.accelerometerXYZ.add(Pair<int, coord>(millis(), accelerometerXYZ), false);

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
		_sampleSet.pressureSamples.add(Pair<int, float>(millis(), pressureValue), false);

		float pressureTemperatureValue = _pressureProvider.getTemperature();
		_sampleSet.pressureTemperatureSamples.add(Pair<int, float>(millis(), pressureTemperatureValue), false);

		float pressureAltitudeValue = _pressureProvider.getAltitude();
		_sampleSet.pressureAltitudeSamples.add(Pair<int, float>(millis(), pressureAltitudeValue), false);

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
		_sampleSet.rainSamples.add(Pair<int, float>(millis(), rainValue), false);

		debugD("Rain Sensor Values: %s", String(rainValue).c_str());

		_lastCheck[Devices::RAIN] = millis();

		debugD("Rain check end: %lu\n", millis());
	}

	// Temperature / Humidity.
	if (_online[Devices::TEMPERATURE_HUMIDITY] && (TIME_DELTA(_lastCheck[Devices::TEMPERATURE_HUMIDITY]) >= UPDATE_RATE_MS(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Temperature / humidity check start: %lu\n", millis());

		float temperatureValue = _temperatureHumidityProvider.getTemperature();
		_sampleSet.temperatureSamples.add(Pair<int, float>(millis(), temperatureValue), false);

		float humidityValue = _temperatureHumidityProvider.getHumidity();
		_sampleSet.humiditySamples.add(Pair<int, float>(millis(), humidityValue), false);

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
		_sampleSet.waterTemperatureSamples.add(Pair<int, float>(millis(), waterTemperature), false);

		debugD("Water Temperature Sensor Values: %s", String(waterTemperature).c_str());

		_lastCheck[Devices::WATER_TEMPERATURE] = millis();

		debugD("Water temperature check end: %lu\n", millis());
	}

	// Wind Direction.
	if (_online[Devices::WIND_DIRECTION] && (TIME_DELTA(_lastCheck[Devices::WIND_DIRECTION]) >= UPDATE_RATE_MS(WIND_DIRECTION_UPDATE_RATE_HZ))) {
		debugD("----------------------");
		debugD("Wind direction check start: %lu\n", millis());

		int windHeadingRaw = _windDirectionProvider.getHeading();
		_sampleSet.windDirectionSamples.add(Pair<int, float>(millis(), windHeadingRaw), false);

		debugD("Wind Direction Sensor Values:");
		debugD("  Wind Heading Raw %d", windHeadingRaw);
		debugD("  Wind Heading Corrected %d", windHeadingRaw);

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
		_sampleSet.windSpeedSamples.add(Pair<int, windspeedpoint>(millis(), windspeedpoint(windSpeedCorrected, windSensorTemperature)), false);

		debugD("Wind Speed Sensor Values:");
		debugD("  Wind Speed Raw (Knots) %s", String(windSpeedRaw).c_str());
		debugD("  Wind Sensor Temperature (Celcius) %s", String(windSensorTemperature).c_str());
		debugD("  Wind Speed Temperature Corrected %s", String(windSpeedCorrected).c_str());

		_lastCheck[Devices::WIND_SPEED] = millis();

		debugD("Wind speed / temperature check end: %lu\n", millis());
	}

	// Local Screen Report.
	if (_screenOn && TIME_DELTA(_screenLastUpdate) >= UPDATE_RATE_MS(SCREEN_UPDATE_RATE_HZ)) {
		String rainState = _rainProvider.getRainState(_sampleSet);
		debugD("Rain state: %s", rainState.c_str());

		String ambientLightState = _ambientLightProvider.getAmbientLightState(_sampleSet);
		debugD("Ambient light state: %s", ambientLightState.c_str());
		_screenLastUpdate = millis();
	}

	//// Transmit Data to "Ground" Station.
	//if (TIME_DELTA(_radioLastTransmit) >= MAX_SEND_INTERVAL_MS) {
	//	debugD("Radio transmit start: %lu\n", millis());

	//	// TODO radio transmit all buffered data.

	//	_radioLastTransmit = millis();

	//	debugD("Radio transmit end: %lu\n", millis());
	//}

	// Restart the station...
	///soft_restart();
}
