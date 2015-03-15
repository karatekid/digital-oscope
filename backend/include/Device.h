#ifndef __DEVICE__H__
#define __DEVICE__H__
//Boost Libraries
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

//Digilent Library
#include <digilent/waveforms/dwf.h>

//User Libraries
#include "AnalogInput.h"
#include "DigitalInput.h"

class Device {
	public:
		//Members
		//DigitalInput digitalIn;
		AnalogInput analogIn;
		//Functions

		//Lock the device as a reader
		/*
		HDWF acquireRead();
		//Lock the device as a writer (get exclusive ownership)
		HDWF acquireWrite();
		//Unock device whether reader or writer
		void releaseLock();
		*/
		Device();
		bool isInitialized();
	private:
		HDWF device;
		bool deviceInitialized;
};
#endif
