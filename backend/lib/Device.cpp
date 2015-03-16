#include "Device.h"

Device::Device() {
	deviceInitialized = false;
	//Discover devices
	int numDevices = 0;
	FDwfEnum(enumfilterAll, &numDevices);
	//printf("Num devices: %d\n", numDevices);
	if(numDevices < 1) {
		//printf("No Device attached\n");
		return;
	}
	DEVID deviceId;
	DEVVER deviceVersion;
	bool haveDiscovery = false;
	int devIdx = 0;
	char strVal[32];
	for(int i = 0; i < numDevices; ++i) {
		FDwfEnumDeviceType(i, &deviceId, &deviceVersion);
		//printf("ID: %d, VERSION: %d\n", deviceId, deviceVersion);
		FDwfEnumDeviceName(devIdx, strVal);
		//printf("Device name: %s\n",strVal);
		if(deviceId == devidDiscovery) {
			haveDiscovery = true;
			devIdx = i;
		}
	}
	if(!haveDiscovery) {
		//printf("It's not a discovery, you SUCK!\n");
		return;
	}
	FDwfEnumSN(devIdx, strVal);
	FDwfDeviceOpen(devIdx, &device);
	deviceInitialized = true;

	//Setup Instruments
	analogIn  = AnalogInput(device);
	digitalIn = DigitalInput(device);
}

bool Device::isInitialized() {
	return deviceInitialized;
}

