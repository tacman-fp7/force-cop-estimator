

#include "gp-force-cop-estimator.h"
#include <iostream>
#include <string>
#include <gurls++/gprwrapper.h>
#include <yarp/os/Value.h>
#include <gurls++/wrapper.h>
#include <gurls++/kernelrlswrapper.h>
#include <yarp/os/Network.h>

using namespace gurls;
using std::string;
using yarp::os::Bottle;
using yarp::os::Value;

using std::cout;
using std::cerr;
using std::endl;


namespace tacman {



// Create a ForceReconstruction object for bodypart.
// partName:
ForceReconstruction::ForceReconstruction(yarp::os::ResourceFinder &rf)
    : ContactConditionEstimator(rf)
{
    _gpOpts = NULL;

    if(!init(rf))
    {
        cerr << "ForceReconstruction: failed to initialise." << endl;
    }
}

// Initialise the module
bool ForceReconstruction::init(yarp::os::ResourceFinder &rf)
{


    _dataDir = rf.check("modelPath", Value(""),
                        "data directory (string)").asString();



    string modelName = rf.find("modelFile").asString();
    string modelFileName = "";


    if (modelName.empty())
    {
        cerr << "Warning: no model file was provided. Creating empty model" << endl;
        _gpOpts = new GurlsOptionsList(_dataDir + "/models/" + "defaultFinger", true);
    }
    else
    {

        modelFileName = _dataDir + "/models/" + _whichHand + "_" + _whichFinger + "/" + modelName;
        cout << "Model filename: " << modelFileName << endl;
        _gpOpts = new GurlsOptionsList(modelFileName + ".bin", false);
        try
        {
            _gpOpts->load(modelFileName);
            cout << "loaded the file" << endl;
        }
        catch(...)
        {

            cerr << "Warning: failed to load the model file." << endl;

            delete(_gpOpts);
            return false;
        }

    }

/*    yarp::os::Network::connect(
                "/" + _whichRobot + "/skin/" + _whichHand + "_hand_comp",
                "/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger + "/tactile:i");

*/

    return true;
}

ForceReconstruction::~ForceReconstruction()
{

    //_forceReconstPort.close();
    if(_gpOpts !=NULL)
    {

        delete(_gpOpts);
        _gpOpts = NULL;
    }
}



bool ForceReconstruction::train()
{

    cout << "training" << endl;


    //setCallbackLock();

    gMat2D<double> Xtr, Xte, ytr, yte;

    string trainingDataDir = _dataDir + "/trainingData/" + _whichHand + "_" + _whichFinger;
    string XtrFileName = trainingDataDir + "/Xtr.csv";
    string XteFileName = trainingDataDir + "/Xte.csv";
    string ytrFileName = trainingDataDir + "/ytr.csv";
    string yteFileName = trainingDataDir + "/yte.csv";

    string means_fileName("pred_gp_means.csv");
    string vars_fileName("pred_gp_vars.csv");

    // Load data files
    std::cout << "Loading data files..." << std::endl;
    Xtr.readCSV(XtrFileName);
    Xte.readCSV(XteFileName);
    ytr.readCSV(ytrFileName);
    yte.readCSV(yteFileName);


    ///////////////////////////////////////

    // specify the task sequence
    OptTaskSequence *seq = new OptTaskSequence();
    seq->addTask("split:ho");
    seq->addTask("paramsel:siglamhogpregr");
    seq->addTask("kernel:rbf");
    seq->addTask("optimizer:rlsgpregr");
    seq->addTask("predkernel:traintest");
    seq->addTask("pred:gpregr");




    GurlsOptionsList * process = new GurlsOptionsList("processes", false);

    // defines instructions for training process
    OptProcess* processTraining = new OptProcess();
    *processTraining << GURLS::computeNsave
                     << GURLS::computeNsave
                     << GURLS::computeNsave
                     << GURLS::computeNsave
                     << GURLS::ignore
                     << GURLS::ignore;
    process->addOpt("train", processTraining);

    // defines instructions for testing process
    OptProcess* processEval = new OptProcess();
    *processEval << GURLS::load
                 << GURLS::load
                 << GURLS::load
                 << GURLS::load
                 << GURLS::computeNsave
                 << GURLS::computeNsave;
    process->addOpt("eval", processEval);



    // build an options' structure
    string modelFileName = _dataDir + "/models/" + _whichHand + "_" + _whichFinger + "/GURLSgpr";
    _gpOpts = new GurlsOptionsList(modelFileName, true);
    _gpOpts->addOpt("seq", seq);
    _gpOpts->addOpt("processes", process);

    OptNumber *nholdouts = new OptNumber;
    nholdouts->setValue(2);    if(_gpOpts->hasOpt("nholdouts"))
        _gpOpts->removeOpt("nholdouts");
    _gpOpts->addOpt("nholdouts", nholdouts);

    OptNumber *hoproportion = new OptNumber;
    hoproportion->setValue(0.1);
    if(_gpOpts->hasOpt("hoproportion"))
        _gpOpts->removeOpt("hoproportion");
    _gpOpts->addOpt("hoproportion", hoproportion);

    OptNumber *epochs = new OptNumber;
    epochs->setValue(100000);
    if(_gpOpts->hasOpt("epochs"))
        _gpOpts->removeOpt("epochs");
    _gpOpts->addOpt("epochs", epochs);

    OptString *hoperf = new OptString;
    hoperf->setValue("abserr");
    if(_gpOpts->hasOpt("hoperf"))
        _gpOpts->removeOpt("hoperf");

    _gpOpts->addOpt("hoperf", hoperf);

    OptString *perfeval = new OptString;
    perfeval->setValue("abserr");
    if(_gpOpts->hasOpt("perfeval"))
        _gpOpts->removeOpt("perfeval");
    _gpOpts->addOpt("perfeval", perfeval);



    GURLS G;

    string jobId0("train");


    // run gurls for training
    G.run(Xtr, ytr, *_gpOpts, jobId0);
    _gpOpts->save(_gpOpts->getName());


    return true;
}

double ForceReconstruction::readOption(const std::string &main, const std::string &sub,  GurlsOptionsList* opt)
{

    //        means = opt.main.sub;
    GurlsOptionsList* mainOpt = GurlsOptionsList::dynacast(opt->getOpt(main));
    GurlsOption *subOpt = mainOpt->getOpt(sub);
    const gMat2D<double>& supOpt_mat = OptMatrix<gMat2D<double> >::dynacast(mainOpt)->getValue();
    return supOpt_mat(0,0); //TODO: bounds check!
}

bool ForceReconstruction::estimateContactCondition(Bottle &tactileBottle, Bottle &contactConditionEstimate)
{


   //printf("Time: % 3.0f\n",  (std::clock() - _time) / (double)(CLOCKS_PER_SEC / 1000));
    _time = std::clock();


    // finger data
    gMat2D<double> tactileData;
    double tactileFactor = 1;
    //gMat2D<double> dummy;
    string jobId1("two");

    //dummy.resize(1,1);
    //dummy(0,0) = 0;
    tactileData.resize(1,12);

    double tactileSum = 0;

    for (int i = 0; i < 12; i++){
        tactileData(0, i ) = tactileBottle.get(i).asDouble();
        if(tactileData(0, i ) < 5)
            tactileData(0,i ) = 0;
        else
        {
            tactileData(0,i) *= tactileFactor;
        }
        tactileSum += tactileData(0,i );
    }
    tactileSum /= 12;


    ////////
    // std::cout << "Testing..." << std::endl;
    try{
        gMat2D<double> vars;
        //gMat2D<double>* means = _gpWrapper->eval(tactileData, vars);
        // gurls::GurlsOptionsList opt(_gpWrapper->getOpt());
        gMat2D<double>* means = this->eval(tactileData, vars, _gpOpts);
        //cout << "evaluated";
        ////////

       // Bottle& outBottle = _port_forceCoP_out.prepare();
        contactConditionEstimate.clear();
        contactConditionEstimate.addDouble((*(means->getData()))/10);
        //contactConditionEstimate.addDouble(vars(0,0));
       // outBottle.addDouble(tactileSum);
        // outBottle.addDouble(meansMatrix(0,0));
        //outBottle.addDouble(varsMatrix(0,0));

        //cout << "Writing: " << outBottle.toString() << endl;
        //_port_forceCoP_out.write(true);
        //_port_forceCoP_out.waitForWrite();
    }
    catch(gException& e)
    {

        cerr << e.getMessage() << endl;

    }
    catch(...)
    {

        cout << "something else went wrong!" << endl;
    }



}

gMat2D<double>* ForceReconstruction::eval(const gMat2D<double> &X, gMat2D<double> &vars, gurls::GurlsOptionsList  *opt){

    typedef double T;

    gMat2D<T> empty;
    PredGPRegr<T> predTask;

    PredKernelTrainTest<T> predkTrainTest;

    opt->removeOpt("predkernel");
    opt->addOpt("predkernel", predkTrainTest.execute(X, empty,*opt));

    GurlsOptionsList *pred = predTask.execute(X, empty, *opt);


    //std::cout << pred->toString() << std::endl;
    OptMatrix<gMat2D<T> >* pmeans = pred->getOptAs<OptMatrix<gMat2D<T> > >("means");
    pmeans->detachValue();

    gMat2D<T> &predMeans = pmeans->getValue();
    gMat2D<T> &predVars = pred->getOptValue<OptMatrix<gMat2D<T> > >("vars");

    const unsigned long n = predMeans.rows();
    const unsigned long t = predMeans.cols();


    vars.resize(n, t);

    for(int i = 0; i < n; i++)
    {

        for (int j = 0; j < t; j++)
        {
            vars(i,j) = predVars(i,j);
        }
    }


    delete pred;

    return &predMeans;
}
} // namespace tacman


