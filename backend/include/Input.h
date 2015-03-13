#ifndef __INPUT__H__
#define __INPUT__H__
#include<vector>
#include<cstdint>
/*
 * Abstract Base Class
 */
class Input { 
	public:
		std::vector<uint16_t> data;
		unsigned int numChannels;

		//Configuration functions
		virtual double setFrequency(double f) = 0; //Pure Virtual Function

		//Blocking Processes that either continually get data and
		//populate the data vector or gets a single measurement and
		//puts in data vector
		/*
		void singleRead();
		void continuousSnapReads();
		void completeScanReads();
		*/
	private:
};

#endif
