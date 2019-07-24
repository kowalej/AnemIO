#include "AnemioStation.h"

AnemioStation::AnemioStation() : serial(Serial) {}

void AnemioStation::setup() {
	// Set serial rate.
	Serial.begin(115200);

	// Wait for serial connection to open (only necessary on some boards).
	while (!Serial);

	// Startup i2c interface.
	Wire.begin();

	// Setup pressure sensor.
	pressureProvider.setup();
}

void AnemioStation::loop() {
	serial.printf("Pressure %.2f\n", pressureProvider.getPressure());
	serial.printf("Pressure Sensor Temperature %.2f\n", pressureProvider.getTemperature());
	serial.printf("Altitude (based on pressure) %.2f\n", pressureProvider.getAltitude());
	delay(1000);
}
