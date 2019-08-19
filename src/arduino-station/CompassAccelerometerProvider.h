// CompassAccelerometer.h

#ifndef COMPASS_ACCELEROMETER_h
#define COMPASS_ACCELEROMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IDataProvider.h"
#include <SerialDebug.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include "Coord.h"

class CompassAccelerometerProvider : public IDataProvider {
	public:
		CompassAccelerometerProvider();
		bool setup();

		// Gets the compass heading.
		int getHeading(float x, float y);

		// Gets the X,Y,Z compass values.
		coord getCompass();

		// Gets the accelerometer X,Y,Z values.
		coord getAccelerometer();

	private:
		/// Assign a unique ID to this sensor at the same time.
		Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
		Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
};

#endif

