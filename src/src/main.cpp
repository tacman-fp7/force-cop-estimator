/**
 * Force and CoP reconstruction
 **/

#include <forceCoPEstimation_ANN.h>
#include "gp-force-cop-estimator.h"
#include "neuralNet.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>
#include <iostream>
#include <cstdlib>
#include <yarp/os/Value.h>


using std::cout;
using std::endl;

int main(int argc, char* argv[])
{

    yarp::os::Network yarp;
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("force-cop-estimator.ini");
    rf.setDefaultContext("force-cop-estimator");
    rf.configure(argc, argv);

    string whichMethod;


    whichMethod = rf.check("whichMethod", yarp::os::Value("no-method")).asString();

    cout << "Initialising the module..." <<endl;



    tacman::ForceCoPEstimator* forceCoPEstimator = NULL;
    if(whichMethod.compare("gp-force") == 0){
        cout << "gp-force...";
        forceCoPEstimator = new tacman::ForceReconstruction(rf);
        cout << "done!" << endl;
    }
    else if(whichMethod.compare("gp-cop") == 0){
        cout << "gp-cop...";

        cout << "done!" << endl;
    }
    else if(whichMethod.compare("ann-posTax") == 0){
        cout << "ann-posTax...";
        forceCoPEstimator = new tacman::ForceCoPEstimation_ANN(rf);
        cout << "done!" << endl;

    }
    else if(whichMethod.compare("ann-tax") == 0){
        cout << "ann-tax...";
        forceCoPEstimator = new tacman::ForceCoPEstimation_ANN(rf);
        cout << "done!" << endl;

    }
    else{
        cout << "undefined method ( " << whichMethod << " )" << endl;
    }


    if(forceCoPEstimator != NULL){
        forceCoPEstimator->runModule(rf);

        delete(forceCoPEstimator);
        forceCoPEstimator = NULL;
    }
    else{
        cout << "failed!" << endl;
    }


    return 0;




}
