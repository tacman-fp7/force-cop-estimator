#pragma once

#include "contactConditionEstimator.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>

namespace tacman{

using yarp::os::ResourceFinder;
using yarp::os::Bottle;

class AvrgForceEstimator: public ContactConditionEstimator
{
public:
    AvrgForceEstimator(ResourceFinder& rf);
    bool estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate);

private:
    string _dbgtag;
};

}//end of namespace tacman
