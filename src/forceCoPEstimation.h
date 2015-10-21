#pragma once
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <list>


namespace tacman
{



using yarp::os::Bottle;
using yarp::os::ResourceFinder;
using yarp::os::BufferedPort;
using std::string;
using std::list;





class ForceCoPEstimation: public BufferedPort<Bottle> // Event based buffered port
{
public:
    //ForceCoPEstimation(){}
    ForceCoPEstimation(ResourceFinder& rf);

    // Processs the fingertip data
    virtual void onRead(Bottle& tactileBottle);

private:
    bool init(ResourceFinder& rf);

protected:
    string _dataDir;
    string _robotName;
//    list< fingertip_t > _fingertip_list;

};

} // End of namespace tacman
