

#include "forceCoPEstimation.h"
#include <yarp/os/Value.h>
#include <iostream>

namespace tacman {

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;


bool ForceCoPEstimator::train()
{

    trainModel();
    return true;
}

bool ForceCoPEstimator::quit()
{
    return true;
}

ForceCoPEstimator::ForceCoPEstimator(ResourceFinder &rf)
{
    if(!init(rf))
    {
        cerr << "ForceCoPEstimation: failed to initialise." << endl;
    }
}



void ForceCoPEstimator::onRead(yarp::os::Bottle &tactileBottle)
{


}


bool ForceCoPEstimator::init(ResourceFinder& rf)
{

    _dataDir = rf.check("dataDir", Value(""),
                        "data directory (string)").asString();
    _whichRobot = rf.check("robotName", Value("icub"),
                          "robot name (string)").asString();

    Bottle& bodyParts = rf.findGroup("BodyPart");
    if(bodyParts.isNull())
    {
        cerr << "No body fingertips defined" << endl;
        return false;
    }

    return true;


}

} // end of namespace tacman


