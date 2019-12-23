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
#define RADIO_ENCRYPT

class RadioTransceiver {
	public:
		bool setup();
		bool sendMessageWithAutoWake(int command, const char* message, uint8_t retries = RADIO_RETRY_NUM);
		bool sendMessage(int command, const char* message, uint8_t retries = RADIO_RETRY_NUM);
		Pair<int, int> sendSamples(SampleSet& sampleSet);
		void sleep();
		void wake();

	private:
	#ifdef RADIO_ENABLE_ATC
		RFM69_ATC _radio = RFM69_ATC(RADIO_CS_SLAVE_SELECT_PIN, RADIO_INTERRUPT_PIN, true);
	#else
		RFM69 _radio = RFM69(RADIO_CS_SLAVE_SELECT_PIN, RADIO_INTERRUPT_PIN, true);
	#endif
		bool RadioTransceiver::sendSample(unsigned long timestamp, const char* serializedValue, unsigned long baseTimestamp);
		void RadioTransceiver::sendMessageCompact(const char* message, char* messageBuff, int &numSent, int &numSuccess, bool flush = false);
};

#endif

