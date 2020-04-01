#include "BatteryInfoProvider.h"

BatteryInfoProvider::BatteryInfoProvider() { }

// Voltage divider ratio for battery sense must be (R1 + R2) / R2 - see https://github.com/rlogiacco/BatterySense.
const float VOLTAGE_DIV_RATIO = (BATTERY_LEVEL_R1_OHMS + BATTERY_LEVEL_R2_OHMS) / BATTERY_LEVEL_R2_OHMS;

bool BatteryInfoProvider::setup()
{
	// We will use the sigmoidal function that the battery sense library offers, since it is the most accurate and we have the processing power.
	batteryLevel.begin(MCU_VOLTAGE * 1000, VOLTAGE_DIV_RATIO, &sigmoidal);
	_isOnline = true;
	return _isOnline;
}

float BatteryInfoProvider::getBatteryLevel() {
	debugI("Battery voltage: %d.", batteryLevel.voltage());
	return batteryLevel.level();
}