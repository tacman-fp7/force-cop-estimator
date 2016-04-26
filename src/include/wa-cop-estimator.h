/**
 *  Weighted average cop estimator
 */

#pragma once
#include <contactConditionEstimator.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <array>
#define N_TAXELS 12


namespace tacman{

using yarp::os::Bottle;
using yarp::os::ResourceFinder;
using std::array;


class WACoPEstimator: public ContactConditionEstimator{

public:
    WACoPEstimator(ResourceFinder& rf);
    ~WACoPEstimator(){}
    bool estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate);

private:
    double min(Bottle& x);
    double max(Bottle& x);
    void normalise(Bottle& data, Bottle& normalisedData);
private:

     array< array <double, 3>, N_TAXELS > _taxelLocations;
     string _dbgtag;
};

} // end of namespace
