#include "CompassAccelerometerProvider.h"

CompassAccelerometerProvider::CompassAccelerometerProvider() { }

bool CompassAccelerometerProvider::setup() {
	if (!mag.begin())
	{
		printlnE("Setup failed, cannot find valid LSM303 magnetometer. Check wiring and your LSM303 Interface / I2C Address.");
		_isOnline = false;
	}
	else {
		printlnI("Sensor found: LSM303 magnetometer.");
		_isOnline &= true;
	}

	if (!accel.begin())
	{
		printlnE("Setup failed, cannot find valid LSM303 accelerometer. Check wiring and your LSM303 Interface / I2C Address.");
		_isOnline = false;
	}
	else {
		printlnI("Sensor found: LSM303 accelerometer.");
		_isOnline &= true;
	}
	return _isOnline;
}

float* CompassAccelerometerProvider::getCompass() {
	static float r[3] = { 0,0,0 };
	sensors_event_t event;
	mag.getEvent(&event);
	r[0] = event.magnetic.x;
	r[1] = event.magnetic.y;
	r[2] = event.magnetic.z;
	return r;
}

float* CompassAccelerometerProvider::getAccelerometer() {
	static float r[3] = { 0,0,0 };
	sensors_event_t event;
	accel.getEvent(&event);
	r[0] = event.acceleration.x;
	r[1] = event.acceleration.y;
	r[2] = event.acceleration.z;
	return r;
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

