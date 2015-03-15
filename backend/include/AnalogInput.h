#ifndef __ANALOGINPUT__H__
#define __ANALOGINPUT__H__
#include "Input.h"
class AnalogInput : public Input {
	public:
		AnalogInput(HDWF inDev);
		AnalogInput();
		double setFrequency(double f);
		int setBufferSize(int n);
		void resetParameters();
		void configure(bool reconfigure, bool start);
		void useTheseChannels(uint16_t bitOn);
		void setMode(oscope::InputMode::type);
	private:
		//Pure virtual fxn impls
		DwfState status(bool readData);
		int statusSamplesLeft();
		int statusSamplesValid();
		int statusIndexWrite();
		oscope::ADCVals statusPortData(int port, int getN);
		int numBits();
		//Analog Specific functions
		double channelRange(int channel);
		double channelOffset(int channel);
		int numChannels();
};

int16_t doubleToI16(double val, double step, double offset);



#endif
