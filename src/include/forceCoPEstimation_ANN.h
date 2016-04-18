#pragma once
#include "forceCoPEstimation.h"
#include <yarp/os/ResourceFinder.h>
#include "neuralNet.h"
#include <vector>
#include <yarp/os/BufferedPort.h>

namespace tacman
{


using yarp::os::Bottle;
using yarp::os::BufferedPort;


struct fingertip
{
    int startIndex;
    string fingerName;
    string modelFile_CoP;
    string modelFile_force;
    string modelFile_activeTaxel;
    BufferedPort<Bottle> *dataPort;
    tacman::NeuralNet *model_CoP;
    tacman::NeuralNet *model_force;
    tacman::NeuralClassifier *model_activeTaxel;

};


typedef struct fingertip fingertip_t;
typedef vector<fingertip_t> fingertipList_t;

using yarp::os::ResourceFinder;

class ForceCoPEstimation_ANN: public ForceCoPEstimator
{
public:
    ForceCoPEstimation_ANN(ResourceFinder& rf);
    void onRead(Bottle& tactileBottle);

private:
    bool init(ResourceFinder& rf);
    //fingertipList_t _fingertip_list;
    fingertip_t fingertip;

};

}
