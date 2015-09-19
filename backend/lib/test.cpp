/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

 * File Name : test.cpp

 * Purpose : Test Device

 * Creation Date : 19-09-2015

 * Created By : Michael Christen

 _._._._._._._._._._._._._._._._._._._._._.*/

//Standard Libraries
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <ctime>

//Generated Libraries
#include <thrift/TToString.h>
#include "gen-cpp/Oscope.h"
#include "gen-cpp/oscope_constants.h"

//User Libraries
#include "Device.h"

using namespace std;
using namespace ::oscope;

int main(int argc, char **argv) {
    Device dev;
    if(!dev.isInitialized()) {
        printf("Not Connected\n");
        return 1;
    }
    double frequency = 400000;
    int bufSize = 4000;
    dev.digitalIn.resetParameters();
    double digitalFreq = dev.digitalIn.setFrequency(frequency);
    bufSize = dev.digitalIn.setBufferSize(bufSize);
    printf("Freq: %f, buf: %d\n", digitalFreq, bufSize);
    dev.digitalIn.setMode(InputMode::Continuous);
    dev.digitalIn.useTheseChannels(0xFFFF);

    sleep(1);

    dev.digitalIn.start();

    for(;;) {
        dev.digitalIn.read();
        if(dev.digitalIn.data.size()) {
            vector<ADCVals> digitalResults = dev.digitalIn.clearData();
            printf("Hey, internal size: %d\n", digitalResults[0].vals.size());
            for(int i = 0; i < digitalResults[0].vals.size(); ++i) {
                if(digitalResults[0].vals[i] == 0) {
                    printf("0\n");
                }
                //printf("%x\n",digitalResults[0].vals[i]);
            }
        }
    }

    return 0;
}
