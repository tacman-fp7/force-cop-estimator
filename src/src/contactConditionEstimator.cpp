#include "contactConditionEstimator.h"
#include <yarp/os/Value.h>


namespace tacman {
using yarp::os::Value;

ContactConditionEstimator::ContactConditionEstimator(yarp::os::ResourceFinder &rf){

    _whichHand = rf.check("hand", Value("unknown")).asString();
    _whichFinger = rf.check("finger", Value("unknown")).asString();
}

ContactConditionEstimator::~ContactConditionEstimator(){

}

bool ContactConditionEstimator::train()
{
    return true;
}

bool ContactConditionEstimator::estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate){

    return true;
}
} //end of namespace
