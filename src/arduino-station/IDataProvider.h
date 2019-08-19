// IDataProvider.h

#ifndef IDATA_PROVIDER_H
#define IDATA_PROVIDER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class IDataProvider {
	public:
		virtual ~IDataProvider() {}
		// Sets up the device. Returns true after succesful completion.
		virtual bool setup() = 0;
		// Checks if the underlying device is online (can get data).
		bool isOnline() { return _isOnline; };
		void setOnline() { _isOnline = true; }
		void setOffline() { _isOnline = false; }

	protected:
		// Current online state.
		bool _isOnline = false;
};

#endif
