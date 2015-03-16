/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

 * File Name : 1.cpp

 * Purpose :

 * Creation Date : 15-03-2015

 * Created By : Michael Christen

 _._._._._._._._._._._._._._._._._._._._._.*/
#include "Input.h"

Input::Input(HDWF inDev) 
	:portsInUse(0), lastWriteIdx(0), device(inDev){
}
Input::Input() 
	:portsInUse(0), lastWriteIdx(0){
}

void Input::start() {
	configure(true, true);
	data = std::vector<oscope::ADCVals>(portsInUse,oscope::ADCVals());
	//data.resize(portsInUse);
}

void Input::singleRead() {
	while(status(true) && !statusSamplesValid()){
		continuousSnapRead();
	}
}

void Input::continuousSnapRead() {
	DwfState devState = status(true);
	/*
	printf("state: %d, Left: %d, Valid: %d, index: %d\n",
			devState,
			statusSamplesLeft(),
			statusSamplesValid(),
			statusIndexWrite());
			*/
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
	printf("index: %d\n", index);
	for(int i = 0; i < portsInUse; ++i) {
		appendFromItoJ(tmpData[i].vals, data[i].vals, lastWriteIdx, index);
		data[i].step = tmpData[i].step;
		data[i].base = tmpData[i].base;
	}
	lastWriteIdx = index;
}

std::vector<oscope::ADCVals> Input::clearData() {
	std::vector<oscope::ADCVals> temp = data;
	for(int i = 0; i < data.size(); ++i) {
		data[i].vals.clear();
	}
	return temp;
}

void Input::read() {
	switch(curMode) {
		case oscope::InputMode::Single:
			singleRead();
			break;
		case oscope::InputMode::Snapshots:
			continuousSnapRead();
			break;
		case oscope::InputMode::Continuous:
			completeScanRead();
			break;
	}
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


