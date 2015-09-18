/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

 * File Name : 1.cpp

 * Purpose :

 * Creation Date : 13-03-2015

 * Created By : Michael Christen

 _._._._._._._._._._._._._._._._._._._._._.*/
#include "DigitalInput.h"

DigitalInput::DigitalInput(HDWF inDev)
	:Input(inDev), bitMask(0) {
	FDwfDigitalIOOutputEnableSet(inDev, 0);
}

DigitalInput::DigitalInput()
	:Input(), bitMask(0) {
}

double DigitalInput::setFrequency(double f) {
	double internal_f;
	FDwfDigitalInClockSourceSet(device, DwfDigitalInClockSourceInternal);
	FDwfDigitalInInternalClockInfo(device, &internal_f);
	FDwfDigitalInDividerSet(device, (unsigned int) std::max(1.0, (internal_f / f)));
	unsigned int div;
	FDwfDigitalInDividerGet(device, &div);
	return internal_f / div;
}

int DigitalInput::setBufferSize(int n) {
	FDwfDigitalInBufferSizeSet(device, n);
	FDwfDigitalInBufferSizeGet(device, &n);
	bufSize = n;
	return n;
}

void DigitalInput::resetParameters() {
	//FDwfDigitalIOReset(device);
	FDwfDigitalInReset(device);
	//unsigned int val;
	//FDwfDigitalIOInputInfo(device, &val);
	//printf("input info: %u\n",val);
}

void DigitalInput::configure(bool reconfigure, bool start) {
	FDwfDigitalInConfigure(device, reconfigure, start);
	//FDwfDigitalIOConfigure(device);
}

void DigitalInput::setMode(oscope::InputMode::type inputType) {
	ACQMODE acqMode;
	switch(inputType) {
		case oscope::InputMode::Single:
			acqMode = acqmodeSingle;
			break;
		case oscope::InputMode::Snapshots:
			acqMode = acqmodeScanShift;
			break;
		case oscope::InputMode::Continuous:
			acqMode = acqmodeScanScreen;
			break;
		default:
			acqMode = acqmodeSingle;
			break;
	}
	FDwfDigitalInAcquisitionModeSet(device, acqMode);
	FDwfDigitalInSampleModeSet(device, DwfDigitalInSampleModeSimple);
	curMode = inputType;
}

//TODO
void DigitalInput::useTheseChannels(uint16_t bitOn) {
	//Get value as uint16
	FDwfDigitalInSampleFormatSet(device, 16);
	portsInUse = 0;
	for(int i = 0; i < numBits(); ++i) {
		if(bitOn & (1 << i)) {
			portsInUse = 1;
		}
	}
	bitMask = bitOn;
}


DwfState DigitalInput::status(bool readData) {
	//FDwfDigitalIOStatus(device);
	//unsigned int val;
	//FDwfDigitalIOInputStatus(device, & val);
	//printf("Input status: %u\n",val);
	DwfState state;
	FDwfDigitalInStatus(device, readData, &state);
	return state;
}

int DigitalInput::statusSamplesLeft() {
	int n;
	FDwfDigitalInStatusSamplesLeft(device, &n);
	return n;
}

int DigitalInput::statusSamplesValid() {
	int n;
	FDwfDigitalInStatusSamplesValid(device, &n);
	return n;
}

int DigitalInput::statusIndexWrite() {
	int n;
	FDwfDigitalInStatusIndexWrite(device, &n);
	return n;
}

oscope::ADCVals DigitalInput::statusPortData(int port, int getN) {
	if(port > 0) {
		assert(false);
	}
	oscope::ADCVals tempADC = oscope::ADCVals();
	tempADC.step  = 1;
	tempADC.base  = 0;

	int copySize = bufSize; //std::min(bufSize, statusSamplesValid());
	int16_t * data = new int16_t[copySize];
	FDwfDigitalInStatusData(device, data, copySize); 
	tempADC.vals.resize(copySize);
	for(int i = 0; i < copySize; ++i) {
		tempADC.vals[i] = bitMask & data[i];
	}
	delete[] data;
	return tempADC;
}

int DigitalInput::numBits() {
	int numBits;
	FDwfDigitalInBitsInfo(device, &numBits);
	return numBits;
}


