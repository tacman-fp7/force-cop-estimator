#pragma once
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <list>
#include <forceCoPController.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Port.h>
#include "contactConditionEstimator.h"

namespace tacman
{



using yarp::os::Bottle;
using yarp::os::ResourceFinder;
using yarp::os::BufferedPort;
using std::string;
using std::list;
using yarp::os::RFModule;
using yarp::os::Port;



class ForceCoPEstimator: public forceCoPController, public BufferedPort<Bottle>, public RFModule // Event based buffered port
{
public:
    ForceCoPEstimator(ResourceFinder& rf);

    // Processs the fingertip data
    virtual void onRead(Bottle& tactileBottle);
    virtual bool train();
    bool quit();

private:
    bool init(ResourceFinder& rf);
    bool init_estimator(ResourceFinder &rf, string estimatorCat, ContactConditionEstimator** estimator);


public: // Methods related to the RF module
    virtual bool attach(yarp::os::Port &source);
    virtual bool configure( yarp::os::ResourceFinder &rf );
    virtual bool updateModule();


private:
    string _dataDir;
    string _whichRobot;
    string _whichHand;
    string _whichFinger;
    int _startIndex;
    //string _whichMethod;
    BufferedPort<Bottle> _port_force_out;
    BufferedPort<Bottle> _port_cop_out;
    BufferedPort<Bottle> _port_acitveTaxelProb_out;

    ContactConditionEstimator *_forceEstimator;
    ContactConditionEstimator *_CoPEstimator;
    ContactConditionEstimator *_activeTaxelEstimator;

    string _dbgtag;


private:
    Port _rpcPort_in;




};

} // End of namespace tacman
