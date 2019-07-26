#include "AnemioStation.h"

AnemioStation::AnemioStation() : _serial(Serial) {}

void AnemioStation::setup() {
	// Set serial rate.
	Serial.begin(115200);

	// Wait for serial connection to open (only necessary on some boards).
	while (!Serial);

	// Startup i2c interface.
	Wire.begin();

	// TODO: check setup completes and return info to radio.

	int numberOffline;
	int retries = 1;
	bool ok;

	do {
		numberOffline = 0;

		// Setup pressure sensor.
		if (!_pressureProvider.isOnline()) {
			ok = _pressureProvider.setup();
			Serial.println(ok ? "Pressure sensing now online" : "Problem brining pressure sensing online, will retry.");
			numberOffline += ok ? 0 : 1;
		}

		// Setup rain sensor.
		if (!_rainProvider.isOnline()) {
			ok = _rainProvider.setup();
			Serial.println(ok ? "Rain sensing now online" : "Problem bringing rain sensing online, will retry.");
			numberOffline += ok ? 0 : 1;
		}

		// Setup temperature humidity sensor.
		if (!_temperatureHumidityProvider.isOnline()) {
			ok = _temperatureHumidityProvider.setup();
			Serial.println(ok ? "Temperature / humidity sensing now online" : "Problem bringing temperature / humidity sensing online, will retry.");
			numberOffline += ok ? 0 : 1;
		}

		retries += 1;
	} while (numberOffline > 0 && retries < 10);

	_serial.printf("Setup complete with %d device(s) offline.", numberOffline);
}

void AnemioStation::loop() {
	/*serial.println("----------------------");
	_serial.println("Pressure Sensor Values:");
	_serial.printf("  Pressure (Pascals) %.2f\n", _pressureProvider.getPressure());
	_serial.printf("  Temperature (Celcius) %.2f\n", _pressureProvider.getTemperature());
	_serial.printf("  Altitude Estimation (Meters) %.2f\n", _pressureProvider.getAltitude());

	_serial.println("----------------------");
	_serial.println("Rain Sensor Values:");*/
	_serial.println(_rainProvider.getRainValue());

	delay(100);
}
