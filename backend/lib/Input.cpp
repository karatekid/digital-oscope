/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

 * File Name : 1.cpp

 * Purpose :

 * Creation Date : 15-03-2015

 * Created By : Michael Christen

 _._._._._._._._._._._._._._._._._._._._._.*/
#include "Input.h"

Input::Input(HDWF inDev) 
	:portsInUse(0), device(inDev){
}
Input::Input() 
	:portsInUse(0){
}

void Input::start() {
	configure(true, true);
	data = std::vector<oscope::ADCVals>(portsInUse,oscope::ADCVals());
	//data.resize(portsInUse);
}

void Input::continuousSnapRead() {
	DwfState devState = status(true);
	assert(data.size() >= portsInUse);
	for(int i = 0; i < portsInUse; ++i) {
		data[i] = statusPortData(i, bufSize);
	}
}

void Input::completeScanRead() {
	DwfState devState = status(true);
	std::vector<oscope::ADCVals> tmpData = std::vector<oscope::ADCVals>(portsInUse, oscope::ADCVals());
	for(int i = 0; i < portsInUse; ++i) {
		tmpData[i] = statusPortData(i, bufSize);
	}
	int index = statusIndexWrite();
	for(int i = 0; i < portsInUse; ++i) {
		appendFromItoJ(tmpData[i].vals, data[i].vals, lastWriteIdx, index);
		data[i].step = tmpData[i].step;
		data[i].base = tmpData[i].base;
	}
	lastWriteIdx = index;
}

void appendFromItoJ(const std::vector<int16_t> &from, 
		std::vector<int16_t> &to,
		int i,
		int j) {
	if(j > i) {
		to.reserve(to.size() + (j - i));
		to.insert(to.end(), from.begin()+i, from.begin() + j);
	} else {
		to.reserve(to.size() + j + (from.size() - i));
		//get backend
		to.insert(to.end(), from.begin()+i, from.end());
		//then wraparound
		to.insert(to.end(), from.begin(), from.begin() + j);
	}
}


