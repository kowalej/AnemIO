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
		String retryMsg = retries < numberOfRetries ? "will retry in a moment" : "will not retry";

		// Setup pressure sensor.
		if (!_online[Devices::PRESSURE]) {
			if ((_online[Devices::PRESSURE] = _pressureProvider.setup())) {
				printlnD("Pressure sensing now online.");
			}
			else {
				debugD("Problem bringing pressure sensing online, %s.", retryMsg);
			}
		}
		numberOffline += _online[Devices::PRESSURE] ? 0 : 1;

		// Setup rain sensor.
		if (!_online[Devices::RAIN]) {
			if ((_online[Devices::RAIN] = _rainProvider.setup())) {
				printlnD("Rain sensing now online.");
			}
			else {
				debugD("Problem bringing rain sensing online, %s.", retryMsg);
			}
		}
		numberOffline += _online[Devices::RAIN] ? 0 : 1;

		// Setup temperature / humidity sensor.
		if (!_online[Devices::TEMPERATURE_HUMIDITY]) {
			if ((_online[Devices::TEMPERATURE_HUMIDITY] = _temperatureHumidityProvider.setup())) {
				printlnD("Temperature / humidity sensing now online.");
			}
			else {
				debugD("Problem bringing temperature / humidity sensing online, %s.", retryMsg);
			}
		}
		numberOffline += _online[Devices::TEMPERATURE_HUMIDITY] ? 0 : 1;

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
}

void AnemioStation::loop() {
	// Pressure.
	if (_online[Devices::PRESSURE] && (millis() - _lastCheck[Devices::PRESSURE] > UPDATE_RATE_MS(PRESSURE_UPDATE_RATE_HZ))) {
		printlnD("----------------------");
		debugD("Pressure check start: %lu\n", millis());

		float pressureValue = _pressureProvider.getPressure();
		_sampleSet.pressureSamples.push(&Pair<float, int>(millis(), pressureValue));

		float pressureTemperatureValue = _pressureProvider.getTemperature();
		_sampleSet.pressureTemperatureSamples.push(&Pair<int, float>(millis(), pressureTemperatureValue));

		float pressureAltitudeValue = _pressureProvider.getAltitude();
		_sampleSet.pressureAltitudeSamples.push(&Pair<int, float>(millis(), pressureAltitudeValue));

		printlnD("Pressure Sensor Values:");
		debugD("  Pressure (Pascals) %s", String(pressureValue).c_str());
		debugD("  Temperature (Celcius) %s", String(pressureTemperatureValue).c_str());
		debugD("  Altitude Estimation (Meters) %s", String(pressureAltitudeValue).c_str());

		_lastCheck[Devices::PRESSURE] = millis();

		debugD("Pressure check end: %lu", millis());
	}

	// Rain.
	if (_online[Devices::RAIN] && (millis() - _lastCheck[Devices::RAIN] > UPDATE_RATE_MS(RAIN_UPDATE_RATE_HZ))) {
		printlnD("----------------------");
		debugD("Rain check start: %lu\n", millis());

		float rainValue = _rainProvider.getRainValue();
		debugD("Rain Sensor Values: %s", String(rainValue).c_str());

		_lastCheck[Devices::RAIN] = millis();

		debugD("Rain check end: %lu\n", millis());
	}

	// Temperature / Humidity.
	if (_online[Devices::TEMPERATURE_HUMIDITY] && (millis() - _lastCheck[Devices::TEMPERATURE_HUMIDITY] > UPDATE_RATE_MS(TEMPERATURE_HUMIDITY_UPDATE_RATE_HZ_NORMAL))) {
		printlnD("----------------------");
		debugD("Temperature / humidity check start: %lu\n", millis());

		float tempValue = _temperatureHumidityProvider.getTemperature();
		float humidityValue = _temperatureHumidityProvider.getHumidity();

		debugD("Temperature / Humidity Sensor Values");
		debugD("  Temperature (Celcius) %s", String(tempValue).c_str());
		debugD("  Humidity (%%) %s", String(humidityValue).c_str());

		_lastCheck[Devices::TEMPERATURE_HUMIDITY] = millis();

		debugD("Temperature / humidity check end: %lu\n", millis());
	}

	// Restart the station...
	///soft_restart();
}
