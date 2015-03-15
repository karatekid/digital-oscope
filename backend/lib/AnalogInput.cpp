/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

 * File Name : 1.cpp

 * Purpose :

 * Creation Date : 13-03-2015

 * Created By : Michael Christen

 _._._._._._._._._._._._._._._._._._._._._.*/
#include "AnalogInput.h"

AnalogInput::AnalogInput(HDWF inDev)
	:Input(inDev) {
}

AnalogInput::AnalogInput()
	:Input() {
}

double AnalogInput::setFrequency(double f) {
	FDwfAnalogInFrequencySet(device, f);
	FDwfAnalogInFrequencyGet(device, &f);
	return f;
}

int AnalogInput::setBufferSize(int n) {
	FDwfAnalogInBufferSizeSet(device, n);
	FDwfAnalogInBufferSizeGet(device, &n);
	bufSize = n;
	return n;
}

void AnalogInput::resetParameters() {
	FDwfAnalogInReset(device);
}

void AnalogInput::configure(bool reconfigure, bool start) {
	FDwfAnalogInConfigure(device, reconfigure, start);
}

DwfState AnalogInput::status(bool readData) {
	DwfState state;
	FDwfAnalogInStatus(device, readData, &state);
	return state;
}

int AnalogInput::statusSamplesLeft() {
	int n;
	FDwfAnalogInStatusSamplesLeft(device, &n);
	return n;
}

int AnalogInput::statusSamplesValid() {
	int n;
	FDwfAnalogInStatusSamplesValid(device, &n);
	return n;
}

int AnalogInput::statusIndexWrite() {
	int n;
	FDwfAnalogInStatusIndexWrite(device, &n);
	return n;
}

oscope::ADCVals AnalogInput::statusPortData(int port, int getN) {
	oscope::ADCVals tempADC = oscope::ADCVals();
	double range  = channelRange(port);
	double offset = channelOffset(port);
	tempADC.step  = range/((1 << numBits() ) - 1);
	tempADC.base  = offset;

	int copySize = std::min(bufSize, statusSamplesValid());
	double * data = new double[copySize];
	FDwfAnalogInStatusData(device,port, data, copySize); 
	tempADC.vals.resize(copySize);
	for(int i = 0; i < copySize; ++i) {
		tempADC.vals[i] = doubleToI16(data[i],tempADC.step,tempADC.base);
	}
	delete[] data;
	return tempADC;
}

int AnalogInput::numBits() {
	int numBits;
	FDwfAnalogInBitsInfo(device, &numBits);
	return numBits;
}

double AnalogInput::channelRange(int channel) {
	double range;
	FDwfAnalogInChannelRangeGet(device, channel, &range);
	return range;
}

double AnalogInput::channelOffset(int channel) {
	double offset;
	FDwfAnalogInChannelOffsetGet(device, channel, &offset);
	return offset;
}

int AnalogInput::numChannels() {
	int num;
	FDwfAnalogInChannelCount(device, &num);
	return num;
}

void AnalogInput::useTheseChannels(uint16_t bitOn) {
	int numCh = numChannels();
	for(int i = 0; i < numCh; ++i) {
		if(bitOn & (1 << i)) {
			FDwfAnalogInChannelEnableSet(device, i, true);
		} else {
			FDwfAnalogInChannelEnableSet(device, i, false);
		}
		//TODO: Range configuration, not sure why 2??
		FDwfAnalogInChannelRangeSet(device, i, 2);
		portsInUse = i+1; //TODO: Assuming for analog just use these to determine length of data?
	}
}

void AnalogInput::setMode(oscope::InputMode::type inputType) {
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
	FDwfAnalogInAcquisitionModeSet(device, acqMode);
	curMode = inputType;
}

int16_t doubleToI16(double val, double step, double offset){
	return (int16_t) ((val - offset)/step);
}
