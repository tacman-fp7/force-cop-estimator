

#include "forceCoPEstimation.h"
#include <yarp/os/Value.h>
#include <iostream>
#include <yarp/os/Network.h>

namespace tacman {

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;


bool ForceCoPEstimator::attach(yarp::os::Port &source)
{

  return this->yarp().attachAsServer(source);

}

bool ForceCoPEstimator::configure(yarp::os::ResourceFinder &rf)
{

    string robotName;
    string whichHand;

    robotName = rf.check("robotName", Value("icubSim")).asString();
    whichHand = rf.check("whichHand", Value("right")).asString();


    useCallback();
    open("/tempForceCoPEstimation");


    //forceCoPEstimator->waitForWrite();

    yarp::os::Network::connect("/" + robotName + "/skin/" + whichHand + "_hand_comp", "/tempForceCoPEstimation");


    _rpcPort_in.open("/cop-force/rpc:i");

    this->attach(_rpcPort_in);

    return true;
}

bool ForceCoPEstimator::updateModule()
{

    return true;
}

bool ForceCoPEstimator::train()
{

    cout << "Training the model" << endl;
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


