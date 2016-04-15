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

using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::ResourceFinder;
using std::string;
using gurls::gMat2D;

namespace tacman {
class ForceReconstruction: public BufferedPort<Bottle>
{
public:

    ForceReconstruction(ResourceFinder& rf);
    ~ForceReconstruction();

    // Train the model
    bool Train(string fingerName);
    // Processs the fingertip data
    virtual void onRead(Bottle& tactileBottle);


private:
    bool init(ResourceFinder& rf);
    double readOption(const string& main, const string& sub,  gurls::GurlsOptionsList *opt);
    gMat2D<double>* eval(const gMat2D<double> &X, gMat2D<double> &vars, gurls::GurlsOptionsList *opt);

private:
    //string _modelDir;
    string _dataDir;
    string _robotName;
    gurls::GurlsOptionsList* _gpOpts;
    BufferedPort _forceReconstPort;
    int _startIndex;
std::clock_t _time;
    gurls::GPRWrapper<double>* _gpWrapper;



    gurls::GURLS _gaussianProcess;
    //gurls::GurlsWrapper<double> *_gurlsWrap;

    //gurls::GPWrapper <double> _gaussianProcess;

};

} // namespace tacman

#endif // FORCERECONSTRUCTION_H
