/**
 * Force and CoP reconstruction
 **/

//#include"contactConditionEstimator.h"
//#include <forceCoPEstimation_ANN.h>
//#include "gp-force-cop-estimator.h"
//#include "neuralNet.h"
#include "forceCoPEstimation.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <yarp/os/Bottle.h>
//#include <cstdlib>
//#include <yarp/os/Value.h>


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





    cout << "Initialising the module..." <<endl;

    tacman::ForceCoPEstimator forceCoPModule(rf);

    if(!forceCoPModule.configure(rf)){
        cout << "failed!" << endl;
        return EXIT_FAILURE;
    }
    else{
        cout << "done!" << endl;
        forceCoPModule.runModule();
    }



    return EXIT_SUCCESS;




}
