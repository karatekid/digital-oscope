/**
 * Interface from JS to talk to Digilent Analog Discovery board
 * by Michael Christen
 *
 */


/**
 * Thrift files can namespace, package, or prefix their output in various
 * target languages.
 */
namespace cpp oscope
namespace d oscope
namespace java oscope
namespace php oscope
namespace perl oscope


enum DeviceType {
  DISCOVERY = 1,
  OTHER     = 2
}

struct Limit {
  1: i32 min,
  2: i32 max
}

struct DeviceInfo {
  1: DeviceType type,
  2: map<string,Limit> limits
}

const DeviceInfo DIGILENT_DISCOVERY = {
	'type'   : DeviceType.DISCOVERY,
	'limits' : {
		'frequency': {
			'min' : 0,
			'max' : 100
		},
		'buffer': {
			'min' : 0,
			'max' : 10000
		}
	}
}


struct MeasurementConfig {
}

struct Data {
	1: i32 timestamp,
	2: double value
}

struct ADCVals {
	1: double step,
	2: double base,
	3: list<i16> vals
}

typedef i16 TestType

exception InvalidOperation {
  1: i32 what,
  2: string why
}

service Oscope {

   DeviceInfo ping() throws (1:InvalidOperation err),

   void configMeasurement(1:map<string,MeasurementConfig> configMap) throws (1:InvalidOperation err),

   ADCVals getData() throws (1:InvalidOperation err),

   ADCVals testThroughput(1:i32 n)

}

