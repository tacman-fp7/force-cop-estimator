

#include "forceCoPEstimation.h"
#include <yarp/os/Value.h>
#include <iostream>

namespace tacman {

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;


ForceCoPEstimation::ForceCoPEstimation(ResourceFinder &rf)
{
    if(!init(rf))
    {
        cerr << "ForceCoPEstimation: failed to initialise." << endl;
    }
}



void ForceCoPEstimation::onRead(yarp::os::Bottle &tactileBottle)
{


}


bool ForceCoPEstimation::init(ResourceFinder& rf)
{

    _dataDir = rf.check("dataDir", Value(""),
                        "data directory (string)").asString();
    _robotName = rf.check("robotName", Value("icub"),
                          "robot name (string)").asString();

    Bottle& bodyParts = rf.findGroup("BodyPart");
    if(bodyParts.isNull())
    {
        cerr << "No body fingertips defined" << endl;
        return false;
    }



/*    int nBodyParts = bodyParts.size() -1; // The first element is the group name
    cout << "Number of fingertips: " << nBodyParts << endl;

    if(nBodyParts < 1)
        return false;

    _fingertip_list.resize(nBodyParts);

    //
*/

    return true;


}

} // end of namespace tacman


