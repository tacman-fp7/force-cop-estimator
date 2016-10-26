#include "wa-cop-estimator.h"
#include <string>
#include <yarp/os/Property.h>
//#include "badModelException.h"

namespace tacman {

using yarp::os::Property;
using std::cout;
using std::cerr;
using std::endl;

WACoPEstimator::WACoPEstimator(ResourceFinder &rf)
    :ContactConditionEstimator(rf){


    _dbgtag = "WACoPEstimator: ";

    std::array<double, 3> tempArray;
    tempArray.fill(0);
    _taxelLocations.fill(tempArray);
    // Load the taxel locations
    string taxelLocationsFileName = _whichHand + "Hand_taxelLocations.ini";

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


        //std::cout << taxelCoordinates->toString() << std::endl;
    }

}

bool WACoPEstimator::estimateContactCondition(Bottle &tactileData, Bottle &contactConditionEstimate){

    contactConditionEstimate.clear();

    if(tactileData.size() != N_TAXELS){
        cerr << _dbgtag << "expected " << N_TAXELS << " taxels data got " << tactileData.size() << endl;
        return false;
    }
    Bottle normalisedData;

    normalise(tactileData, normalisedData);

    std::cout << tactileData.toString() << endl;
    std::cout << normalisedData.toString() << endl;

    double x, y, z;
    x = y = z = 0;


    for(int i = 0; i < N_TAXELS; i++){
        x += _taxelLocations.at(i).at(0) * normalisedData.get(i).asDouble();
        y += _taxelLocations.at(i).at(1) * normalisedData.get(i).asDouble();
        z += _taxelLocations.at(i).at(2) * normalisedData.get(i).asDouble();
    }


    std::cout << "x: " << x << " y: " << y << " z: " << z << endl;

    contactConditionEstimate.addDouble(x);
    contactConditionEstimate.addDouble(y);
    contactConditionEstimate.addDouble(z);

    return true;

}

void WACoPEstimator::normalise(Bottle &data, Bottle &normalisedData){


    double sum = 0;
    normalisedData.clear();

    for (int i = 0; i < data.size(); i++){
        if(data.get(i).asDouble() > 2){
        sum += data.get(i).asDouble();
        normalisedData.addDouble(data.get(i).asDouble());
        }else{
          normalisedData.addDouble(0.0);
        }

    }

    if(sum == 0){
        sum = 1;
    }


    for(int i = 0; i < data.size(); i++){
        normalisedData.get(i) = normalisedData.get(i).asDouble()/sum;
        //normalisedData.addDouble(data.get(i).asDouble() / sum);
    }

/*    double minVal = min(data);
    double maxVal = max(data);

    normalisedData.clear();
    if(minVal == maxVal){
        for(int i = 0; i < data.size(); i++){
            normalisedData.addDouble(0);
        }
    }
    else{
        for(int i = 0; i < data.size(); i++)
            normalisedData.addDouble((data.get(i).asDouble() - minVal)/(maxVal - minVal));
    }*/


}

double WACoPEstimator::min(Bottle &x){

    if(x.size() < 1){
        return 0;
    }

    double min = x.get(0).asDouble();
    for (int i = 1; i < x.size(); i++){
        if(min > x.get(i).asDouble()){
            min = x.get(i).asDouble();
        }
    }


    return min;

}

double WACoPEstimator::max(Bottle &x){
    if(x.size() < 1){
        return 0;
    }

    double max = x.get(0).asDouble();
    for(int i = 1; i < x.size(); i++){
        if(max < x.get(i).asDouble()){
            max = x.get(i).asDouble();
        }
    }
    return max;

}
} // end of namespace
