#include "avrg-force-estimator.h"

namespace tacman {

AvrgForceEstimator::AvrgForceEstimator(ResourceFinder &rf)
    : ContactConditionEstimator(rf){
    _dbgtag = "AvrgForceEstimator: ";
}

bool AvrgForceEstimator::estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate){

    contactConditionEstimate.clear();
    if (tactileData.size() == 0){
        contactConditionEstimate.addDouble(0);
        return false;
    }

    double avrg = 0;

    for(int i =0; i < tactileData.size(); i++){
        avrg += tactileData.get(i).asDouble();
    }

    avrg = avrg/tactileData.size();

    contactConditionEstimate.addDouble(avrg);
    return true;
}
}
