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

    cout << "Initialising the module" << endl;



    //tacman::ForceCoPEstimator* forceCoPEstimator = new tacman::ForceCoPEstimation_ANN(rf);
    tacman::ForceCoPEstimator* forceCoPEstimator = new tacman::ForceReconstruction(rf);

    //forceCoPEstimator->useCallback();
    //forceCoPEstimator->open("/tempForceCoPEstimation");


    //forceCoPEstimator->waitForWrite();

    //yarp::os::Network::connect("/icub/skin/left_hand_comp", "/tempForceCoPEstimation");

    return forceCoPEstimator->runModule(rf);

   // while(true)
   //     ;

    //return EXIT_SUCCESS;
}
