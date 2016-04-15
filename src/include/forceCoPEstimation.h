#pragma once
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <list>
#include <forceCoPController.h>

namespace tacman
{



using yarp::os::Bottle;
using yarp::os::ResourceFinder;
using yarp::os::BufferedPort;
using std::string;
using std::list;





class ForceCoPEstimator: public forceCoPController, public BufferedPort<Bottle> // Event based buffered port
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

protected:
    string _dataDir;
    string _whichRobot;
    string _whichHand;
    string _whichFinger;

};

} // End of namespace tacman
