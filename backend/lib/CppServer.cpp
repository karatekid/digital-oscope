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


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace ::oscope;

class OscopeHandler : virtual public OscopeIf {
 public:
  OscopeHandler() {
    // Your initialization goes here
  }

  void ping(DeviceInfo& _return) {
    // Your implementation goes here
	_return = oscopeConstants().DIGILENT_DISCOVERY;
    printf("ping\n");
  }

  void configMeasurement(const std::map<std::string, MeasurementConfig> & configMap) {
    // Your implementation goes here
    printf("configMeasurement\n");
  }

  void getData(std::vector<Data> & _return) {
    // Your implementation goes here
	_return = std::vector<Data>();
	Data d  = Data();
	d.__set_value(5);
	d.__set_timestamp(0);
	_return.push_back(d);
    printf("getData\n");
  }

};


/*
class CalculatorHandler : public CalculatorIf {
 public:
  CalculatorHandler() {}

  void ping() {
    cout << "ping()" << endl;
  }

  int32_t add(const int32_t n1, const int32_t n2) {
    cout << "add(" << n1 << ", " << n2 << ")" << endl;
    return n1 + n2;
  }

  int32_t calculate(const int32_t logid, const Work& work) {
    cout << "calculate(" << logid << ", " << work << ")" << endl;
    int32_t val;

    switch (work.op) {
    case Operation::ADD:
      val = work.num1 + work.num2;
      break;
    case Operation::SUBTRACT:
      val = work.num1 - work.num2;
      break;
    case Operation::MULTIPLY:
      val = work.num1 * work.num2;
      break;
    case Operation::DIVIDE:
      if (work.num2 == 0) {
        InvalidOperation io;
        io.what = work.op;
        io.why = "Cannot divide by 0";
        throw io;
      }
      val = work.num1 / work.num2;
      break;
    default:
      InvalidOperation io;
      io.what = work.op;
      io.why = "Invalid Operation";
      throw io;
    }

    SharedStruct ss;
    ss.key = logid;
    ss.value = to_string(val);

    log[logid] = ss;

    return val;
  }

  void getStruct(SharedStruct &ret, const int32_t logid) {
    cout << "getStruct(" << logid << ")" << endl;
    ret = log[logid];
  }

  void zip() {
    cout << "zip()" << endl;
  }

protected:
  map<int32_t, SharedStruct> log;

};
*/

int main(int argc, char **argv) {

  int numDevices = 0;
  FDwfEnum(enumfilterAll, &numDevices);
  HDWF device;
  FDwfDeviceOpen(-1, &device);

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
