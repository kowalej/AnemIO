#include "CompassAccelerometerProvider.h"

CompassAccelerometerProvider::CompassAccelerometerProvider() { }

bool CompassAccelerometerProvider::setup() {
	_isOnline = true;
	if (!mag.begin())
	{
		debugE("Setup failed, cannot find valid LSM303 magnetometer. Check wiring and your LSM303 Interface / I2C Address.");
		_isOnline = false;
	}
	else {
		debugI("Sensor found: LSM303 magnetometer.");
		_isOnline = _isOnline && true;
	}

	if (!accel.begin())
	{
		debugE("Setup failed, cannot find valid LSM303 accelerometer. Check wiring and your LSM303 Interface / I2C Address.");
		_isOnline = false;
	}
	else {
		debugI("Sensor found: LSM303 accelerometer.");
		_isOnline = _isOnline && true;
	}
	return _isOnline;
}

coord CompassAccelerometerProvider::getCompass() {
	coord xyz;
	sensors_event_t event;
	mag.getEvent(&event);
	xyz.x = event.magnetic.x;
	xyz.y = event.magnetic.y;
	xyz.z = event.magnetic.z;
	return xyz;
}

coord CompassAccelerometerProvider::getAccelerometer() {
	coord xyz;
	sensors_event_t event;
	accel.getEvent(&event);
	xyz.x = event.acceleration.x;
	xyz.y = event.acceleration.y;
	xyz.z = event.acceleration.z;
	return xyz;
}

int CompassAccelerometerProvider::getHeading(float x, float y) {
	// Calculate the angle of the vector y,x.
	float heading = (atan2(y, x) * 180) / PI;

	// Normalize to 0-360.
	if (heading < 0)
	{
		heading = 360 + heading;
	}

	return (int)heading;
}

