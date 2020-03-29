// BatteryInfoProvider.h

#ifndef BATTERY_INFO_PROVIDER_H
#define BATTERY_INFO_PROVIDER_H

#include "IDataProvider.h"
#include "Constants.h"

class BatteryInfoProvider : public IDataProvider
{
public:
	BatteryInfoProvider();
	bool setup();
	// Get battery level (percent).
	float getBatteryLevel();

private:
	int _numConsecutiveBatteryLevelFaults = 0;
	void checkFaults();
};

#endif