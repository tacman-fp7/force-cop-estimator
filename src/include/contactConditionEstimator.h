#pragma once
#include <yarp/os/Thread.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <string>
// This the main controller

namespace tacman {

using yarp::os::Thread;
using yarp::os::Bottle;
using std::string;

class ContactConditionEstimator{

public:
   ContactConditionEstimator(yarp::os::ResourceFinder &rf);
   ~ContactConditionEstimator();


   virtual bool train();
   virtual bool estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate);
   //virtual void run(){}

protected:
   string _whichHand;
   string _whichFinger;

};
} // end of namesapce
