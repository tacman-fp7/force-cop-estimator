#pragma once
#include "forceCoPEstimation.h"
#include <yarp/os/ResourceFinder.h>
#include "neuralNet.h"
#include <exception>
//#include <yarp/os/BufferedPort.h>
//#include "contactConditionEstimator.h"

namespace tacman
{


using yarp::os::Bottle;
using yarp::os::BufferedPort;


using yarp::os::ResourceFinder;



class ForceCoPEstimation_ANN: public ContactConditionEstimator
{
public:
    ForceCoPEstimation_ANN(ResourceFinder& rf);
    bool estimateContactCondition(Bottle &tactileBottle, Bottle &contactConditionEstimate);


private:
    bool init(ResourceFinder& rf);
    string _modelFile;
    NeuralNet *_model;
};

}
