#ifndef AnemioStation_H
#define AnemioStation_H

#include "PressureProvider.h"
#include <PrintEx.h>
#include <Wire.h>

class AnemioStation {
    public:
        AnemioStation();
        void setup();
	    void loop();

	private:
		PrintEx serial; //Wrap the Serial object in a PrintEx interface.
		PressureProvider pressureProvider;
};

#endif
