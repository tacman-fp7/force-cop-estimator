#pragma once
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <list>
#include <forceCoPController.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Port.h>

namespace tacman
{



using yarp::os::Bottle;
using yarp::os::ResourceFinder;
using yarp::os::BufferedPort;
using std::string;
using std::list;
using yarp::os::RFModule;




class ForceCoPEstimator: public forceCoPController, public BufferedPort<Bottle>, public RFModule // Event based buffered port
{
public:
    ForceCoPEstimator(ResourceFinder& rf);

    // Processs the fingertip data
    virtual void onRead(Bottle& tactileBottle);
    virtual void trainModel(){}
    bool train();
    bool quit();

private:
    bool init(ResourceFinder& rf);

public: // Methods related to the RF module
    virtual bool attach(yarp::os::Port &source);
    virtual bool configure( yarp::os::ResourceFinder &rf );
    virtual bool updateModule();
    //bool close();

protected:
    string _dataDir;
    string _whichRobot;
    string _whichHand;
    string _whichFinger;

private:
    yarp::os::Port _rpcPort_in;

};

} // End of namespace tacman
