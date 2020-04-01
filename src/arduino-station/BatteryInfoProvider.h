// BatteryInfoProvider.h

#ifndef BATTERY_INFO_PROVIDER_H
#define BATTERY_INFO_PROVIDER_H

#include "IDataProvider.h"
#include "Constants.h"
#include <Battery.h>

class BatteryInfoProvider : public IDataProvider
{
public:
	BatteryInfoProvider();
	bool setup();
	// Get battery level (percent).
	float getBatteryLevel();

private:
	Battery batteryLevel = Battery(BATTERY_DISCHARGED_MILLIVOLTS, BATTERY_FULLCHARGED_MILLIVOLTS, BATTERY_LEVEL_INPUT_PIN);
};

#endif