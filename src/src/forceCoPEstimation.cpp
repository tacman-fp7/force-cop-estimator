

#include "forceCoPEstimation.h"
#include <yarp/os/Value.h>
#include <iostream>
#include <yarp/os/Network.h>
#include "gp-force-cop-estimator.h"
#include "forceCoPEstimation_ANN.h"
#include "wa-cop-estimator.h"
#include "avrg-force-estimator.h"

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

    bool ret = true;

    ////////////// Initialise the force estimator ///////////
    ret = ret && init_estimator(rf, "ForceEstimator", &_forceEstimator);

    /////////////    Initialise the CoP estimator  //////////
    ret = ret && init_estimator(rf, "CoPEstimator", &_CoPEstimator);

    //////// Initialise the active taxel estimator //////////
    ret = ret && init_estimator(rf, "ActiveTaxelEstimator", &_activeTaxelEstimator);

    //////////// Initialise this module /////////
    ret = ret && init(rf);


    return ret;
}

bool ForceCoPEstimator::init_estimator(ResourceFinder &rf, string estimatorCat, ContactConditionEstimator** estimator){

    ResourceFinder rfEstimator = rf.findNestedResourceFinder(estimatorCat.c_str());
    if(rfEstimator.isNull()){
        cerr << _dbgtag << "no estimator configuration data found ( " << estimatorCat << " ): " << endl;
        return false;
    }

    // Get the estimator type
    string estimatorType = rfEstimator.find("estimatorType").asString();
    //    string modelFile = rfForceEstimator.find("modelFile").asString();

    try{
    if(estimatorType.compare("gp") == 0){
        *estimator = new ForceReconstruction(rfEstimator);
    }
    else if(estimatorType.compare("ann") ==  0){
        *estimator = new ForceCoPEstimation_ANN(rfEstimator);
    }
    else if(estimatorType.compare("avrg") == 0 ){
        if(estimatorCat.compare("CoPEstimator") == 0)
        {
            *estimator = new WACoPEstimator(rfEstimator);
        }
        else if(estimatorCat.compare("ForceEstimator") == 0){
            *estimator = new AvrgForceEstimator(rfEstimator);
        }
        else{
            cerr << _dbgtag << "failed to load " << estimatorType << " model." << endl;
        }
    }
    else{
        cerr << _dbgtag << "failed to load " << estimatorType << " model." << endl;
        return false;
    }
    }catch(std::exception& e){
        cerr << _dbgtag << e.what() << endl;
        return false;
    }

    return true;


}



bool ForceCoPEstimator::updateModule()
{

    return true;
}

bool ForceCoPEstimator::train()
{

    cout << "No training method implemented" << endl;

    return false;
}

bool ForceCoPEstimator::quit()
{
    return true;
}

ForceCoPEstimator::ForceCoPEstimator(ResourceFinder &rf)
{

    _dbgtag = "forceCoPEstimator: ";

    _forceEstimator = NULL;
    _CoPEstimator = NULL;
    _activeTaxelEstimator = NULL;
}



void ForceCoPEstimator::onRead(yarp::os::Bottle &tactileBottle)
{


    Bottle featureVect;

    // Prepare the bottles for the ports
    Bottle& forceOut = _port_force_out.prepare();
    Bottle& copOut = _port_cop_out.prepare();
    Bottle& activeTaxelOut = _port_acitveTaxelProb_out.prepare();

    forceOut.clear();
    copOut.clear();
    activeTaxelOut.clear();

    // The tactile data is for all of the fingers. Take only the ones
    // for this finger
    featureVect.copy(tactileBottle, _startIndex, 12);


    _CoPEstimator->estimateContactCondition(featureVect, copOut);
    _forceEstimator->estimateContactCondition(featureVect, forceOut );
    _activeTaxelEstimator->estimateContactCondition(featureVect, activeTaxelOut);

    double maxProb = 0;
    int maxProbIndex = 0;
    for(int i = 0; i < activeTaxelOut.size(); i++){
        if(activeTaxelOut.get(i).asDouble() > maxProb){
            maxProb = activeTaxelOut.get(i).asDouble();
            maxProbIndex = i;
        }
    }

    activeTaxelOut.addInt(maxProbIndex + 1);
    _port_acitveTaxelProb_out.writeStrict();
    _port_cop_out.writeStrict();
    _port_force_out.writeStrict();


    _port_cop_out.waitForWrite();
    _port_force_out.waitForWrite();
    _port_acitveTaxelProb_out.waitForWrite();

}


bool ForceCoPEstimator::init(ResourceFinder& rf)
{

    _dataDir = rf.check("dataDir", Value(""),
                        "data directory (string)").asString();


    _whichRobot = rf.check("robotName", Value("unknown")).asString();
    _whichHand = rf.check("whichHand", Value("unknown")).asString();
    _whichFinger = rf.check("fingerName", Value("unknown")).asString();
    _startIndex = rf.check("startIndex", Value(-1)).asInt();

    if(_startIndex == -1){
        cerr << _dbgtag << "start index was missing in the config file. Settingit to 0." << endl;
        return false;
    }

    RFModule::setName( rf.check("moduleName", Value("unknown-module")).asString().c_str());
    useCallback();

    string tactilePortLocal = "/" + RFModule::getName()  + "/"  + _whichHand + "_" + _whichFinger +
            "/tactile:i";

    string tactilePortRemote = "/" + _whichRobot + "/skin/" + _whichHand + "_hand_comp";

    open(tactilePortLocal);


    _port_force_out.open("/" + RFModule::getName() + "/" + _whichHand + "_" + _whichFinger +
                         "/force:o");

    _port_cop_out.open("/" + RFModule::getName() + "/" + _whichHand + "_" + _whichFinger +
                       "/cop:o");
    _port_acitveTaxelProb_out.open("/" + RFModule::getName() + "/" + _whichHand + "_" + _whichFinger +
                                   "/activeTaxelProb:o");



    _rpcPort_in.open("/" + RFModule::getName()   + "/" + _whichHand + "_" + _whichFinger + "/rpc:i");

    this->attach(_rpcPort_in);



    if(!yarp::os::Network::connect(tactilePortRemote, tactilePortLocal)){
        cerr << _dbgtag << "Could not connect to the remote port: " << tactilePortRemote << endl;
        return false;
    }




    return true;


}

} // end of namespace tacman


