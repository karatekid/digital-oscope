#ifndef __DIGITALINPUT__H__
#define __DIGITALINPUT__H__
#include "Input.h"
class DigitalInput : public Input {
	public:
		DigitalInput(HDWF inDev);
		DigitalInput();
		double setFrequency(double f);
		int setBufferSize(int n);
		void resetParameters();
		void configure(bool reconfigure, bool start);
		void useTheseChannels(uint16_t bitOn);
		void setMode(oscope::InputMode::type);
	private:
		uint16_t bitMask;
		//Pure virtual fxn impls
		DwfState status(bool readData);
		int statusSamplesLeft();
		int statusSamplesValid();
		int statusIndexWrite();
		oscope::ADCVals statusPortData(int port, int getN);
		int numBits();
		//Digital Specific Functions

};

#endif
