#ifndef __DEVICE__H__
#define __DEVICE__H__
#include "AnalogInput.h"
#include "DigitalInput.h"
class Device {
	public:
		//Device();
	private:
		DigitalInput digitalIn;
		AnalogInput analogIn;
};
#endif
