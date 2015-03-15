#ifndef __INPUT__H__
#define __INPUT__H__
#include<vector>
#include<cstdint>
#include <iostream>
#include <cassert>

#include <digilent/waveforms/dwf.h>

#include "gen-cpp/Oscope.h"
#include "gen-cpp/oscope_constants.h"
/*
 * Abstract Base Class
 */
class Input { 
	public:
		//data is split into ports, digital has 1, analog has 2
		std::vector<oscope::ADCVals> data; 
		int numPorts;

		//Main functions
		Input(HDWF inDev);
		Input();

		void start();
		void read(); // does one of the following depeding on current mode
		void continuousSnapRead();
		void completeScanRead();
		//Useful when doing scan read
		std::vector<oscope::ADCVals> clearData();
		void singleRead();
		/*
		*/
		//Configuration functions, return actual set parameter
		virtual double setFrequency(double f) = 0; //Pure Virtual Function
		virtual int setBufferSize(int n) = 0;
		virtual void resetParameters() = 0;
		virtual void configure(bool reconfigure, bool start) = 0;
		virtual void useTheseChannels(uint16_t bitOn) = 0;
		virtual void setMode(oscope::InputMode::type) = 0;
		
		//void startAcquisition();
		//void stopAcquisition();
	protected:
		//Members
		HDWF device;
		//Device dev; //Used to guarantee atomicity on device
		int portsInUse;
		int bufSize;
		int lastWriteIdx;
		oscope::InputMode::type curMode;

	private:
		virtual DwfState status(bool readData) = 0;
		//Status must be called before all of these
		virtual int statusSamplesLeft() = 0;
		virtual int statusSamplesValid() = 0;
		virtual int statusIndexWrite() = 0;
		virtual oscope::ADCVals statusPortData(int port, int getN) = 0;
		//^done with needing to call status
		virtual int numBits() = 0;
		//set with bitmap of channels

		//Private Members

};

void appendFromItoJ(const std::vector<int16_t> &from, 
		std::vector<int16_t> &to,
		int i,
		int j);

#endif
