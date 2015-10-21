/**
 * Force and CoP reconstruction
 **/

#include "forceCoPEstimation_ANN.h"
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
    rf.setDefaultConfigFile("forceCoPEstimation.ini");
    rf.setDefaultContext("forceCoPEstimation");
    rf.configure(argc, argv);

    cout << "Initialising the module" << endl;

     //tacman::NeuralNet myNet(rf);

    tacman::ForceCoPEstimation* forceCoPEstimator = new tacman::ForceCoPEstimation_ANN(rf);


    forceCoPEstimator->useCallback();
    forceCoPEstimator->open("/tempForceCoPEstimation");

    //forceCoPEstimator->waitForWrite();

    yarp::os::Network::connect("/icubSim/skin/left_hand_comp", "/tempForceCoPEstimation");

    while(true)
        ;

    return EXIT_SUCCESS;
}
