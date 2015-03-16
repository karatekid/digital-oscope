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

//Boost Libraries
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

//Digilent Library
#include <digilent/waveforms/dwf.h>

//Standard Libraries
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <ctime>

//User Libraries
#include "Device.h"


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace ::oscope;

class OscopeHandler : virtual public OscopeIf {
 protected:
	TestType *throughputArray;
	int throughputSize;
	InvalidOperation DEV_NOT_FOUND;
 public:
	static int channel;
	static int bufSize;
	static double frequency;
	static ADCVals trueADC;
	static Device dev;
  OscopeHandler() {
	  DEV_NOT_FOUND = InvalidOperation();
	  DEV_NOT_FOUND.why = "Device not found";
	  //Create Testing array
	  throughputSize = 1000000;
	  throughputArray = new TestType[throughputSize];
	  for(int i = 0; i < throughputSize; ++i) {
		  throughputArray[i] = (TestType)rand();
	  }
	  //Setup device (don't worry, static)
	  //Analog
	  frequency = 100000;
	  channel = 0; //0-indexed
	  bufSize = 10000;
	  dev.analogIn.resetParameters();
	  frequency = dev.analogIn.setFrequency(frequency);
	  bufSize   = dev.analogIn.setBufferSize(bufSize);
	  dev.analogIn.setMode(InputMode::Snapshots);
	  dev.analogIn.useTheseChannels(1 << channel); //Channel 0

	  //Digital
	  dev.digitalIn.resetParameters();
	  double digitalFreq = dev.digitalIn.setFrequency(400000);
	  int bufSize = dev.digitalIn.setBufferSize(10000);
	  printf("Freq: %f, buf: %d\n", digitalFreq, bufSize);
	  dev.digitalIn.setMode(InputMode::Single);
	  dev.digitalIn.useTheseChannels(0x8001); //All

	  sleep(1);

	  //Start
	  dev.analogIn.start();
	  dev.digitalIn.start();
  }

  void ping(DeviceInfo& _return) {
	  // Your implementation goes here
	  if(dev.isInitialized()) {
		  _return = oscopeConstants().DIGILENT_DISCOVERY;
		  printf("ping\n");
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void configMeasurement(const std::map<std::string, MeasurementConfig> & configMap) {
      // Your implementation goes here
	  //Start stuff
	  if(dev.isInitialized()) {
		  
		  printf("configMeasurement\n");
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void getData(ADCVals & _return) {
	  if(dev.isInitialized()) {
		  _return = trueADC;
	  } else {
		  throw DEV_NOT_FOUND;
	  }
  }

  void testThroughput(ADCVals & _return, const int32_t n) {
	_return.vals.resize(n);
	for(int32_t i = 0; i < n; ++i) {
		_return.vals[i] = doubleToI16((double)rand(), 0.125, -2.5);
	}
  }

  static void discoveryProcessor() {
	  boost::posix_time::millisec workTime(max(bufSize/frequency * 1000 - 1000,0.0)); //or millisec
	  if(dev.isInitialized()) {
		  for(;;) {
			  dev.analogIn.read();
			  if(dev.analogIn.data.size()) {
				  trueADC = dev.analogIn.data[0];
			  }

			  dev.digitalIn.read();
			  if(dev.digitalIn.data.size()) {
				  vector<ADCVals> digitalResults = dev.digitalIn.clearData();
				  /*
				  printf("Hey, internal size: %d\n", digitalResults[0].vals.size());
				  for(int i = 0; i < digitalResults[0].vals.size(); ++i) {
					  printf("%x\n",digitalResults[0].vals[i]);
				  }
				  */
			  }

			  //TODO:UnLock
			  boost::this_thread::sleep(workTime);
		  }
	  }
  }

  ~OscopeHandler() {
	  FDwfDeviceCloseAll();
  }

};

int OscopeHandler::channel = 0;
int OscopeHandler::bufSize = 0;
double OscopeHandler::frequency = 10000;
ADCVals OscopeHandler::trueADC = ADCVals();
Device OscopeHandler::dev = Device();


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

	cout << "Starting the Discovery thread..." << endl;
	boost::thread discoveryThread(OscopeHandler::discoveryProcessor);
	cout << "Starting the server..." << endl;
	server.serve();
	cout << "Done." << endl;
	return 0;
}
