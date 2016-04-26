#include "wa-cop-estimator.h"
#include <string>
#include <yarp/os/Property.h>
//#include "badModelException.h"

namespace tacman {

using yarp::os::Property;

WACoPEstimator::WACoPEstimator(ResourceFinder &rf)
    :ContactConditionEstimator(rf){


    std::array<double, 3> tempArray;
    tempArray.fill(0);
    _taxelLocations.fill(tempArray);
    // Load the taxel locations
    string taxelLocationsFileName = _whichHand + "Hand_" + _whichFinger + "Finger_taxelLocations.ini";

    string taxelLocationFile = rf.findFileByName(taxelLocationsFileName);

    if(taxelLocationFile.empty())
    {
        // Throw an exception to let the user know something has gone wrong
    }


    Property modelConf;
    modelConf.fromConfigFile(taxelLocationFile);

    // Read the taxel locations

    Bottle* taxelLocations = modelConf.find("taxelLocations").asList();

    for (int i = 0; i < taxelLocations->size(); i++ ){
        Bottle* taxelCoordinates = taxelLocations->get(i).asList();

        _taxelLocations.at(i).at(0) = taxelCoordinates->get(0).asDouble();
        _taxelLocations.at(i).at(1) = taxelCoordinates->get(1).asDouble();
        _taxelLocations.at(i).at(2) = taxelCoordinates->get(2).asDouble();


        std::cout << taxelCoordinates->toString() << std::endl;
    }

}




} // end of namespace
