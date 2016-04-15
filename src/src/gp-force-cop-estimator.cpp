

#include "forcereconstruction.h"
#include <iostream>
#include <string>
#include <gurls++/gprwrapper.h>
#include <yarp/os/Value.h>
#include <gurls++/wrapper.h>
#include <gurls++/kernelrlswrapper.h>


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
{
    _gpOpts = NULL;

    if(!init(rf))
    {
        cerr << "ForceReconstruction: failed to initialise." << endl;
    }
};

// Initialise the module
bool ForceReconstruction::init(yarp::os::ResourceFinder &rf)
{


    _dataDir = rf.check("dataDir", Value(""),
                        "data directory (string)").asString();
    _robotName = rf.check("robotName", Value("icub"),
                          "robot name (string)").asString();
    string moduleName = rf.check("moduleName", Value("force-reconstruction"),
                                 "module name (string)").asString();

    //
    Bottle& bodyParts = rf.findGroup("BodyPart");
    if(bodyParts.isNull())
    {
        cerr << "No body parts defined" << endl;
        return false;
    }



    int nBodyParts = bodyParts.size() -1; // The first element is the group name
    cout << "Number of body parts: " << nBodyParts << endl;


    /*    for (int i = 1; i <= nBodyParts; i++) // The first element is the group name
    {
        cout << bodyParts.get(i).toString() << endl;

        // Read the data
        Bottle& subPart = rf.findGroup(bodyParts.get(i).asString());
        cout << subPart.toString() << endl;
    }
*/

    int i = 1;
    Bottle& subPart = rf.findGroup(bodyParts.get(i).asString());
    string fingerName = subPart.find("fingerName").asString();


    if (fingerName.empty())
    {
        cerr << "No finger defined" << endl;
        return false;
    }

    _startIndex = subPart.check("startIndex", Value(-1)).asInt();
    if(_startIndex == -1)
    {

        cerr <<"Warning: no start index was defined using defalut value of 0" << endl;
        _startIndex = 0;
    }

    string modelName = subPart.find("modelName").asString();
    string modelFileName = "";

    if (modelName.empty())
    {
        cerr << "Warning: no model file was provided. Creating empty model" << endl;
        _gpOpts = new GurlsOptionsList(_dataDir + "/models/" + "defaultFinger", true);
    }
    else
    {

        modelFileName = _dataDir + "/models/" + fingerName + "/" + modelName;
        _gpOpts = new GurlsOptionsList(modelFileName + ".bin", false);
        try
        {
            _gpOpts->load(modelFileName);
            cout << "loaded the file" << endl;
        }
        catch(...)
        {

            cerr << "failed to load the model file" << endl;
            delete(_gpOpts);
            cerr << "Warning: no model file was provided. Creating empty model" << endl;
            _gpOpts = new GurlsOptionsList(_dataDir + "/models/" + "defaultFinger", true);
        }

        //OptNumber *n = new OptNumber(0);
        //if(_gpOpts->hasOpt("todisk"))
        //{

            //_gpOpts->removeOpt("todisk");
            //_gpOpts->addOpt("todisk", new OptNumber(0));


        //}
        //else
        //{
         //   _gpOpts->addOpt("todisk", n);


        //}

       // if(n!=NULL){
       //delete(n);
       // n=NULL;}


       _gpWrapper = new GPRWrapper<double>(modelFileName);


       //////////////////////

/*****
        _gpWrapper->setKernelType(GPRWrapper<double>::RBF);
        _gpWrapper->setProblemType(GPRWrapper<double>::REGRESSION);
        _gpWrapper->setNparams(20);
        _gpWrapper->setNSigma(25);

******/

/*
               gMat2D<double> Xtr, Xte, ytr, yte;

               string trainingDataDir = _dataDir + "/trainingData/" + fingerName;
               string XtrFileName = trainingDataDir + "/Xtr.csv";
               string XteFileName = trainingDataDir + "/Xte.csv";
               string ytrFileName = trainingDataDir + "/ytr.csv";
               string yteFileName = trainingDataDir + "/yte.csv";

               // Load data files
               std::cout << "Loading data files..." << std::endl;
               Xtr.readCSV(XtrFileName);
               Xte.readCSV(XteFileName);
               ytr.readCSV(ytrFileName);
               yte.readCSV(yteFileName);
*/
               // Initialize model
              // std::cout << "Training model..." << std::endl;
               //_gpWrapper->train(Xtr, ytr);

               //_gpWrapper->saveModel(modelFileName + ".bin");
              /**  _gpWrapper->loadOpt(modelFileName + ".bin");



                gurls::GurlsOptionsList opt(_gpWrapper->getOpt());

                if(opt.hasOpt("meanX"))
                    cout << "Has the opt" << endl; **/

                //cout << opt.toString();
                //_gpWrapper->
               //cout << _gpOpts->toString() << endl;
               // _gpWrapper->loadOpt(*_gpOpts);


       /////////////////
      /*  // Trying
        if (_gpOpts->hasOpt("kernel"))
        {
            using  gurls::KernelRLSWrapper;

            _gurlsWrap = new KernelRLSWrapper<double>("evaluation");
            std::string kernelType = _gpOpts->getOptValue<OptString>("kernel.type");
            if (kernelType=="rbf")
                dynamic_cast<KernelRLSWrapper<double>*>(_gurlsWrap)->setKernelType(KernelWrapper<double>::RBF);
        }

        // end of tyring
*/
        _forceReconstPort.open("/" + moduleName + "/" + fingerName + "/force-CoP" );
    }
    return true;
}

ForceReconstruction::~ForceReconstruction()
{

    _forceReconstPort.close();
    if(_gpOpts !=NULL)
    {

        delete(_gpOpts);
        _gpOpts = NULL;
    }
}



bool ForceReconstruction::Train(string fingerName)
{

    gMat2D<double> Xtr, Xte, ytr, yte;

    string trainingDataDir = _dataDir + "/trainingData/" + fingerName;
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

    // specify the task sequence
    OptTaskSequence *seq = new OptTaskSequence();
    *seq << "split:ho" << "paramsel:siglamhogpregr" << "kernel:rbf"
         << "optimizer:rlsgpregr" << "predkernel:traintest" << "pred:gpregr";

    GurlsOptionsList * process = new GurlsOptionsList("processes", false);

    // defines instructions for training process
    OptProcess* process1 = new OptProcess();
    *process1 << GURLS::computeNsave << GURLS::computeNsave <<  GURLS::computeNsave
              <<  GURLS::computeNsave << GURLS::ignore << GURLS::ignore;
    process->addOpt("one", process1);

    // defines instructions for testing process
    OptProcess* process2 = new OptProcess();
    *process2 << GURLS::load << GURLS::load <<  GURLS::load <<  GURLS::load
              <<GURLS::computeNsave << GURLS::computeNsave;

     process->addOpt("two", process2);
   OptProcess* process3 = new OptProcess();
    *process3 << GURLS::load << GURLS::load <<  GURLS::load <<  GURLS::load
              <<GURLS::computeNsave << GURLS::computeNsave;


    process->addOpt("eval", process3);


    // build an options' structure
    string modelFileName = _dataDir + "/models/" + fingerName + "/GURLSgpr";
    GurlsOptionsList* opt = new GurlsOptionsList(modelFileName, true);
    opt->addOpt("seq", seq);
    opt->addOpt("processes", process);



    GURLS G;

    string jobId0("one");
    string jobId1("two");

    // run gurls for training
    G.run(Xtr, ytr, *opt, jobId0);
     opt->save(opt->getName());
    GurlsOptionsList* opt2 = new GurlsOptionsList(modelFileName, false);
    opt2->load(modelFileName + ".bin");
    //run gurls for testing
    G.run(Xte, yte, *opt2, jobId1);



    //        means = opt.pred.means;
    GurlsOptionsList* pred = GurlsOptionsList::dynacast(opt2->getOpt("pred"));
    GurlsOption *means_opt = pred->getOpt("means");

    const gMat2D<double>& means_mat = OptMatrix<gMat2D<double> >::dynacast(means_opt)->getValue();

    means_mat.saveCSV(means_fileName);






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

void ForceReconstruction::onRead(Bottle &tactileBottle)
{


 printf("Time: % 3.0f\n",  (std::clock() - _time) / (double)(CLOCKS_PER_SEC / 1000));
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

    for (int i = _startIndex; i < _startIndex + 12; i++){
        tactileData(1, i - _startIndex) = tactileBottle.get(i).asDouble();
        if(tactileData(1, i - _startIndex) < 5)
            tactileData(1,i - _startIndex) = 0;
        else
        {
        tactileData(1,i - _startIndex) *= tactileFactor;
        }
        tactileSum += tactileData(1,i - _startIndex);
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
   //_gaussianProcess.run(tactileData, dummy, *_gpOpts, jobId1);
   // OptMatrix<gMat2D<double> > *meansMatrix =   _gpOpts->getOptAs< OptMatrix<gMat2D<double> > >("pred.means");
   //gMat2D<double> meansMatrix = (_gpOpts->getOptAs< OptMatrix<gMat2D<double> > >("pred.means"))->getValue();
   //gMat2D<double> varsMatrix = (_gpOpts->getOptAs< OptMatrix<gMat2D<double> > >("pred.vars"))->getValue();


//   gMat2D<double> meansMat = means->getData();
   Bottle& outBottle = _forceReconstPort.prepare();
   outBottle.clear();
   outBottle.addDouble((*(means->getData()))*5);
   outBottle.addDouble(vars(0,0));
   outBottle.addDouble(tactileSum);
  // outBottle.addDouble(meansMatrix(0,0));
   //outBottle.addDouble(varsMatrix(0,0));

   //cout << "Writing: " << outBottle.toString() << endl;
   _forceReconstPort.write(true);
   _forceReconstPort.waitForWrite();
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


