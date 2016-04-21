#ifndef FORCERECONSTRUCTION_H
#define FORCERECONSTRUCTION_H

#include <string>
#include <gurls++/gprwrapper.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <gurls++/gurls.h>
#include <gurls++/wrapper.h>
#include <gurls++/gprwrapper.h>
#include <gurls++/gmat2d.h>
#include "forceCoPEstimation.h"
#include "contactConditionEstimator.h"


using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::ResourceFinder;
using std::string;
using gurls::gMat2D;

namespace tacman {
class ForceReconstruction: public ContactConditionEstimator //ForceCoPEstimator//BufferedPort<Bottle>
{
public:

    ForceReconstruction(ResourceFinder& rf);
    ~ForceReconstruction();

    // Train the model
    bool train();
    // Processs the fingertip data
    bool estimateContactCondition(Bottle &tactileBottle, Bottle &contactConditionEstimate);
    //virtual void onRead(Bottle& tactileBottle);


private:
    bool init(ResourceFinder& rf);
    double readOption(const string& main, const string& sub,  gurls::GurlsOptionsList *opt);
    gMat2D<double>* eval(const gMat2D<double> &X, gMat2D<double> &vars, gurls::GurlsOptionsList *opt);

private:
    string _dataDir;
    gurls::GurlsOptionsList* _gpOpts;
    //int _startIndex;
    std::clock_t _time;


};

} // namespace tacman

#endif // FORCERECONSTRUCTION_H
