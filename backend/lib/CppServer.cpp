/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

//Thrift Libraries
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/THttpServer.h>
#include <thrift/TToString.h>
#include "gen-cpp/Oscope.h"
#include "gen-cpp/oscope_constants.h"

//Digilent Library
#include <digilent/waveforms/dwf.h>

//Standard Libraries
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <ctime>


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace ::oscope;


class OscopeHandler : virtual public OscopeIf {
 protected:
	HDWF device;
	bool deviceInit;
	int channel;
	int bufSize;
	bool *throughputArray;
	int throughputSize;
	InvalidOperation DEV_NOT_FOUND;
 public:
  OscopeHandler() {
	  DEV_NOT_FOUND = InvalidOperation();
	  DEV_NOT_FOUND.why = "Device not found";
	  //Create Testing array
	  throughputSize = 1000000;
	  throughputArray = new bool[throughputSize];
	  for(int i = 0; i < throughputSize; ++i) {
		  if(i%2) {
			  throughputArray[i] = true;
		  } else {
			  throughputArray[i] = false;
		  }
	  }
	  //Setup device
	  int numDevices = 0;
	  FDwfEnum(enumfilterAll, &numDevices);
	  if(numDevices == 0) {
		  /*
		  cerr << "Need an Oscilloscope\n" << endl;
		  exit(1);
		  */
		  deviceInit = false;
		  return;
	  }
	  FDwfDeviceOpen(-1, &device);
	  deviceInit = true;
  }

  void ping(DeviceInfo& _return) {
	  // Your implementation goes here
	  if(deviceInit) {
		  _return = oscopeConstants().DIGILENT_DISCOVERY;
		  printf("ping\n");
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void configMeasurement(const std::map<std::string, MeasurementConfig> & configMap) {
      // Your implementation goes here
	  //Start stuff
	  if(deviceInit) {
		  FDwfAnalogInReset(device);
		  FDwfAnalogInAcquisitionModeSet(device, acqmodeScanShift);
		  double frequency = 100000;
		  channel = 0; //0-indexed
		  bufSize = 10000;

		  FDwfAnalogInFrequencySet(device, frequency);
		  FDwfAnalogInChannelEnableSet(device, channel, true);
		  FDwfAnalogInChannelRangeSet(device, channel, 2);

		  FDwfAnalogInBufferSizeSet(device, bufSize);
		  //Get actual buffer size
		  FDwfAnalogInBufferSizeGet(device, &bufSize);
		  FDwfAnalogInFrequencyGet(device, &frequency);
		  printf("Freq: %f\n", frequency);
		  //FDwfAnalogInRecordLengthSet(device, bufSize / frequency);

		  sleep(1);

		  //Start
		  FDwfAnalogInConfigure(device, true, true);
		  DwfState devState = DwfStateRunning;
		  //Finish
		  /*
			 while(devState != DwfStateDone) {
			 FDwfAnalogInStatus(device, true, &devState);
			 }
			 */
		  printf("configMeasurement\n");
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void getData(std::vector<Data> & _return) {
	  DwfState devState;
	  if(deviceInit) {
		  FDwfAnalogInStatus(device, true, &devState);
		  double * data = new double[bufSize];
		  FDwfAnalogInStatusData(device,channel, data, bufSize); 
		  Data d = Data();
		  for(int i = 0; i < bufSize; ++i) {
			  d.__set_value(data[i]);
			  d.__set_timestamp(i);
			  _return.push_back(d);
			  //printf("%d\t%f\n", i, data[i]);
		  }
		  //_return.assign(data, data + bufSize);
		  delete[] data;
		  //Stop instrument
		  FDwfAnalogInConfigure(device, false, false);
		  printf("getData\n");
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void testThroughput(std::vector<bool> & _return, const int32_t n) {
	//std::clock_t start = clock();
	_return.assign(throughputArray, throughputArray + min(n,throughputSize));
	/*
	std::clock_t end = clock();
	cout << n << "\t" << (end - start + 0.0) << endl;
	*/
  }

  ~OscopeHandler() {
	  FDwfDeviceCloseAll();
  }

};

int main(int argc, char **argv) {
	//Protocol should be JSON
	boost::shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());
	boost::shared_ptr<OscopeHandler> handler(new OscopeHandler());
	boost::shared_ptr<TProcessor> processor(new OscopeProcessor(handler));
	boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(9090));
	//Transport should be ajax
	boost::shared_ptr<TTransportFactory> transportFactory(new THttpServerTransportFactory());

	TSimpleServer server(processor,
			serverTransport,
			transportFactory,
			protocolFactory);

	cout << "Starting the server..." << endl;
	server.serve();
	cout << "Done." << endl;
	return 0;
}
