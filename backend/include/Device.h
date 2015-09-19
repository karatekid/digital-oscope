#ifndef __DEVICE__H__
#define __DEVICE__H__
//Digilent Library
#include <digilent/waveforms/dwf.h>

//User Libraries
#include "AnalogInput.h"
#include "DigitalInput.h"

class Device {
	public:
		//Members
		DigitalInput digitalIn;
		AnalogInput analogIn;

		//Functions
		Device();
		bool isInitialized();
	private:
		HDWF device;
		bool deviceInitialized;
};
#endif
