// RadioTransceiver.h

#ifndef RADIO_TRANSCEIVER_H
#define RADIO_TRANSCEIVER_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Constants.h"
#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include "SerialDebug.h"
#include "SampleSet.h"

// #define RADIO_ENABLE_ATC

class RadioTransceiver {
	public:
		bool setup();
		bool sendMessageWithAutoWake(int command, const char* message);
		bool sendMessage(int command, const char* message);
		bool sendSamples(SampleSet& sampleSet);
		void sleep();
		void wake();

	private:
	#ifdef RADIO_ENABLE_ATC
		RFM69_ATC _radio = RFM69_ATC(RADIO_CS_SLAVE_SELECT_PIN, RADIO_INTERRUPT_PIN, false);
	#else
		RFM69 _radio = RFM69(RADIO_CS_SLAVE_SELECT_PIN, RADIO_INTERRUPT_PIN);
	#endif
		bool sendSample(const long timestamp, const char* serializedValue);
};

#endif

