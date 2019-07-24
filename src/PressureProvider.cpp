#include "PressureProvider.h"

PressureProvider::PressureProvider() : bmx280(BMP280_IC2_ADDRESS) {}

void PressureProvider::setup() {
	// begin() checks the Interface, reads the sensor ID(to differentiate between BMP280 and BME280)
	if (!bmx280.begin())
	{
		Serial.println("Setup failed, cannot find valid BMP or BME 280 device. Check wiring and your BMx280 Interface / I2C Address.");
		while (1);
	}

	if (bmx280.isBME280())
		Serial.println("Sensor found: BME280");
	else
		Serial.println("Sensor found: BMP280");

	// Reset sensor to default parameters.
	bmx280.resetToDefaults();

	// By default sensing is disabled and must be enabled by setting a non-zero oversampling setting.
	// Set an oversampling setting for pressure and temperature measurements. 
	bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

	bmx280.writePowerMode(BMx280MI::power_mode_t::BMx280_MODE_NORMAL);
	bmx280.writeStandbyTime(BMx280MI::standby_time_t::T_SB_2);

	// If sensor is a BME280, set an oversampling setting for humidity measurements.
	if (bmx280.isBME280())
		bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
}

void PressureProvider::measure() {
	// Start a measurement.
	if (!bmx280.measure())
	{
		return;
	}

	// Wait for the measurement to finish.
	do
	{
		delay(100);
	} while (!bmx280.hasValue());

	pressureRead = false;
	temperatureRead = false;
	altitudeRead = false;
}

float PressureProvider::getPressure() {
	if (pressureRead) {
		measure();
	}
	pressureRead = true;
	lastPressure = bmx280.getPressure();
	return lastPressure;
}

float PressureProvider::getTemperature() {
	if (temperatureRead) {
		measure();
	}
	temperatureRead = true;
	return bmx280.getTemperature();
}

float PressureProvider::getAltitude() {
	if (altitudeRead) {
		measure();
	}
	altitudeRead = true;
	
	float altitude;
	float pressure = lastPressure != NAN ? lastPressure : getPressure();
	float pressurehPa = pressure/100; // Convert to hPA (aka millibar).

	// Use crazy formula to get altitude in meters. See https://en.wikipedia.org/wiki/Pressure_altitude.
	altitude = 44307.69396 * (1.0 - pow(pressurehPa / 1013.25, 0.190284));

	return altitude;
}
