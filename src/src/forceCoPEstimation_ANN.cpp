
#include "forceCoPEstimation_ANN.h"
#include <yarp/os/Value.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>

namespace tacman
{

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;
using yarp::os::Property;



ForceCoPEstimation_ANN::ForceCoPEstimation_ANN(yarp::os::ResourceFinder &rf)
    : ForceCoPEstimator(rf)
{

    init(rf);
}

void ForceCoPEstimation_ANN::onRead(yarp::os::Bottle &tactileBottle)
{
    // Tactile data is available
    //for(fingertipList_t::iterator it = _fingertip_list.begin(); it != _fingertip_list.end(); it++)
    //{

    vector<double> cop;
    vector<double> force;
    vector<double> activeTaxels;

    // For each fingetip calculate the cop and force
    // There are 12 taxels per fingure.
    vector<double> featureVector;

    for(int i = fingertip.startIndex; i < fingertip.startIndex + 12; i++)
        featureVector.push_back(tactileBottle.get(i).asDouble());



    //it->model_CoP->feedForward(input);
    fingertip.model_CoP->feedForward(featureVector);
    fingertip.model_CoP->getResults(cop);

    fingertip.model_activeTaxel->feedForward(featureVector);
    fingertip.model_activeTaxel->getResults(activeTaxels);


    // Only add the position data if the model is for more than
    // 12 inputs
    if(fingertip.model_force->getInputSize() > featureVector.size())
    {


        // Add the position estimate to the taxel data
        // Also the position data is scaled by 10000 to make it comparable with force data
        for (vector<double>::iterator it_cop = cop.begin(); it_cop != cop.end(); it_cop++)
        {
            featureVector.push_back((*it_cop) * 10000); // The scale factor
        }
    }
    fingertip.model_force->feedForward(featureVector);
    fingertip.model_force->getResults(force);



    // Publish the data

    Bottle &activeTaxelProbs_out = _port_acitveTaxelProb_out.prepare();
    activeTaxelProbs_out.clear();

    Bottle &forceCoP_out = _port_forceCoP_out.prepare();
    forceCoP_out.clear();
    for (int i = 0; i < force.size(); i++)
        forceCoP_out.addDouble(force.at(i));

    for (int i = 0; i < cop.size(); i++)
        forceCoP_out.addDouble(cop.at(i));



    int maxTaxel = 0;
    double maxTaxelProb = 0;
    for (int i = 0; i < activeTaxels.size(); i++)
    {
        activeTaxelProbs_out.addDouble(activeTaxels.at(i));
        if(maxTaxelProb <= activeTaxels.at(i))
        {
            maxTaxelProb = activeTaxels.at(i);
            maxTaxel = i;
        }
        //forceCoP_out.addDouble(activeTaxels.at(i));
    }


    //cout << "Active taxel: " << maxTaxel + 1 << endl;
    //cout << forceCoP_out.toString() << endl;
    forceCoP_out.addInt(maxTaxel + 1); // Taxel number start from 1

    _port_forceCoP_out.write(true);
    _port_acitveTaxelProb_out.write(true);

    _port_forceCoP_out.waitForWrite();
    _port_acitveTaxelProb_out.waitForWrite();




    //}


}

bool ForceCoPEstimation_ANN::init(ResourceFinder &rf)
{


    string moduleName = rf.check("moduleName", Value("forceCoPEstimation_ann"),
                                 "module name (string)").asString();

    fingertip.fingerName = rf.find("fingerName").asString();

    if (fingertip.fingerName.empty())
    {
        cerr << "No finger defined" << endl;
        return false;
    }

    fingertip.startIndex = rf.check("startIndex", Value(-1)).asInt();
    if(fingertip.startIndex == -1)
    {

        cerr <<"Warning: no start index was defined using defalut value of 0" << endl;
        fingertip.startIndex = 0;
    }

    fingertip.modelFile_CoP = rf.findFileByName(rf.find("model_CoP").asString());

    // Open it
    Property modelConf;
    modelConf.fromConfigFile(fingertip.modelFile_CoP);
    fingertip.model_CoP = new tacman::NeuralNet(modelConf);


    fingertip.modelFile_force = rf.findFileByName(rf.find("model_force").asString());


    modelConf.clear();
    modelConf.fromConfigFile(fingertip.modelFile_force);
    fingertip.model_force = new tacman::NeuralNet(modelConf);


    fingertip.modelFile_activeTaxel = rf.findFileByName(rf.find("model_activeTaxel").asString());

    modelConf.clear();
    modelConf.fromConfigFile(fingertip.modelFile_activeTaxel);
    fingertip.model_activeTaxel = new tacman::NeuralClassifier(modelConf);

    // Open the port to publish CoP and force
    //fingertip.dataPort = new BufferedPort<Bottle>();

    //fingertip.dataPort->open("/" + moduleName + "/" + fingertip.fingerName);


    cout << "Finger Name: " << fingertip.fingerName << endl;
    cout << "Starting index: " << fingertip.startIndex << endl;
    cout << "Model file (CoP): " << fingertip.modelFile_CoP << endl << endl;
    cout << "Model file (force): " << fingertip.modelFile_force << endl << endl;


    yarp::os::Network::connect(
                "/" + _whichRobot + "/skin/" + _whichHand + "_hand_comp",
                "/" + RFModule::getName() + "/" + _whichMethod + "/" + _whichHand + "_" + _whichFinger + "/tactile:i");



    return true;
}

} // namespace tacman
