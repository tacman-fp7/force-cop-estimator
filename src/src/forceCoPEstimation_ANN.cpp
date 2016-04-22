
#include "forceCoPEstimation_ANN.h"
#include <yarp/os/Value.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include "badModelException.h"

namespace tacman
{

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;
using yarp::os::Property;



ForceCoPEstimation_ANN::ForceCoPEstimation_ANN(yarp::os::ResourceFinder &rf)
    :ContactConditionEstimator(rf)
{

    init(rf);
}

bool ForceCoPEstimation_ANN::estimateContactCondition(Bottle &tactileBottle, Bottle &contactConditionEstimate)
{


    vector<double> modelOutput;


    // For each fingetip calculate the cop and force
    // There are 12 taxels per fingure.
    vector<double> featureVector;

    for(int i = 0; i <  12; i++)
        featureVector.push_back(tactileBottle.get(i).asDouble());


    _model->feedForward(featureVector);
    _model->getResults(modelOutput);

    contactConditionEstimate.clear();
    for(vector<double>::iterator i = modelOutput.begin(); i != modelOutput.end(); i++){

        contactConditionEstimate.addDouble(*i);
    }




}

bool ForceCoPEstimation_ANN::init(ResourceFinder &rf)
{


    string modelType = rf.find("modelType").asString();

    _modelFile = rf.findFileByName(rf.find("modelFile").asString());

    if(_modelFile.empty()){
        throw bad_model;
    }

    // Open it
    Property modelConf;
    modelConf.fromConfigFile(_modelFile);
    if(modelType.compare("classifier") == 0){
        _model = new tacman::NeuralClassifier(modelConf);
    }
    else{
        _model = new tacman::NeuralNet(modelConf);
    }

    return true;
}

} // namespace tacman
