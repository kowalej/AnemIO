#pragma region SerialDebug Settings

// SerialDebug Library

// Disable all debug ? Good for release builds (production)
// as nothing from SerialDebug is compiled, zero overhead :-)
// For it just uncomment the DEBUG_DISABLED
// #define DEBUG_DISABLED true

// Define the initial debug level here (uncomment to do it)
#include "RadioTransceiver.h"
#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

// Disable SerialDebug debugger ? No more commands and features as functions and globals
// Uncomment this to disable it
#define DEBUG_DISABLE_DEBUGGER false

// Disable auto function naming (good if your debug message already contains it)
//#define DEBUG_AUTO_FUNC_DISABLED true

// Force debug messages to use flash?
// Disable native Serial.printf (if using)
// Good for low memory, due to flash use, but is slowwer and doesn't use macros
//#define DEBUG_USE_FLASH_F true

#if defined(ARDUINO_AVR_ADK)
#define SERIAL_SPEED 250000
#elif defined(ARDUINO_AVR_MEGA)
#define SERIAL_SPEED 250000
#elif defined(ARDUINO_AVR_MEGA2560)
#define SERIAL_SPEED 250000
#elif defined(ARDUINO_SAM_DUE)
#define SERIAL_SPEED 250000
#elif defined(ARDUINO_SAMD_ZERO)
#define SERIAL_SPEED 250000
#elif defined(ARDUINO_ARC32_TOOLS)
#define SERIAL_SPEED 250000
#elif defined ESP8266 || defined ESP32
#define SERIAL_SPEED 250000
#else
#define SERIAL_SPEED 115200
#endif

#include "SerialDebug.h" 

#pragma endregion

#include "AnemioStation.h"

AnemioStation station;

void setup()
{
	// Set serial rate.
	Serial.begin(SERIAL_SPEED);

#ifdef __AVR_ATmega32U4__ 
	while (!Serial) {} // Wait for serial connection to open (only necessary on some boards).
#else
	delay(500); // Wait a fixed time for serial.
#endif
	station.setup();
}

void loop()
{
	// Handle Serial Debug.
	debugHandle();

	// Main station operation.
	// 1. Check sensor statuses.
	// 2. Collect samples.
	// 3. Send over radio.
	station.loop();

#ifndef DEBUG_DISABLED
	delay(15);
#else
	LowPower.powerSave(period_t::SLEEP_15MS, ADC_ON, BOD_ON, TIMER2_ON);
#endif
}
