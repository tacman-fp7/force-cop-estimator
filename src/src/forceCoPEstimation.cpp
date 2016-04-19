

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

    _whichRobot = rf.check("robotName", Value("unknown")).asString();
    _whichHand = rf.check("whichHand", Value("unknown")).asString();
    _whichFinger = rf.check("fingerName", Value("unknown")).asString();
    _whichMethod = rf.check("whichMethod", Value("unknown")).asString();

    RFModule::setName( rf.check("moduleName", Value("force-cop-estimator")).asString().c_str());
    useCallback();
    open("/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger +
         "/tactile:i");


    _port_forceCoP_out.open("/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger +
                            "/forceCoP:o");
    _port_acitveTaxelProb_out.open("/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger +
                                   "/activeTaxelProb:o");



    _rpcPort_in.open("/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger + "/rpc:i");

    this->attach(_rpcPort_in);


    yarp::os::Network::connect(
                "/" + _whichRobot + "/skin/" + _whichHand + "_hand_comp",
                "/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger + "/tactile:i");

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


