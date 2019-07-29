// SampleSet.h

#ifndef _SAMPLE_SET_h
#define _SAMPLE_SET_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Constants.h"
#include "Pair.h"
#include "cppQueue.h"

class SampleSet {
	public:
		SampleSet();
		Queue pressureSamples = Queue(sizeof(Pair<int, float>), GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, MAX_SEND_INTERVAL_MS), QueueType::FIFO, false);
		Queue pressureTemperatureSamples = Queue(sizeof(Pair<int, float>), GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, MAX_SEND_INTERVAL_MS), QueueType::FIFO, false);
		Queue pressureAltitudeSamples = Queue(sizeof(Pair<int, float>), GET_BUFFER_SIZE(PRESSURE_UPDATE_RATE_HZ, MAX_SEND_INTERVAL_MS), QueueType::FIFO, false);
};

#endif

