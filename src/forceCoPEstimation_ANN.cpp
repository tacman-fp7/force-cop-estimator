
#include "forceCoPEstimation_ANN.h"
#include <yarp/os/Value.h>
#include <yarp/os/Property.h>


namespace tacman
{

using yarp::os::Value;
using std::cout;
using std::cerr;
using std::endl;
using yarp::os::Property;



ForceCoPEstimation_ANN::ForceCoPEstimation_ANN(yarp::os::ResourceFinder &rf)
    : ForceCoPEstimation(rf)
{

    init(rf);
}

void ForceCoPEstimation_ANN::onRead(yarp::os::Bottle &tactileBottle)
{
    // Tactile data is available

    cout << "incoming data" << endl;

    for(fingertipList_t::iterator it; it != _fingertip_list.end(); it++)
    {

        // For each fingetip calculate the cop and force

        vector<double> input;
        for(int i = it->startIndex; i < it->startIndex + 12; i++)
            input.push_back(tactileBottle.get(i).asDouble());

        it->model_CoP->feedForward(input);
        it->model_force->feedForward(input);

        vector<double> cop;
        vector<double> force;

        it->model_CoP->getResults(cop);
        it->model_force->getResults(cop);

        // Publish the data

        Bottle &forceCoP_out = it->dataPort->prepare();

        for (int i = 0; i < cop.size(); i++)
            forceCoP_out.addDouble(cop.at(i));

        for (int i = 0; i < force.size(); i++)
            forceCoP_out.addDouble(force.at(i));


        it->dataPort->write(true);
        it->dataPort->waitForWrite();




    }


}

bool ForceCoPEstimation_ANN::init(ResourceFinder &rf)
{


    string moduleName = rf.check("moduleName", Value("forceCoPEstimation"),
                                 "module name (string)").asString();

    Bottle& bodyParts = rf.findGroup("BodyPart");
    if(bodyParts.isNull())
    {
        cerr << "No body fingertips defined" << endl;
        return false;
    }



    int nBodyParts = bodyParts.size() -1; // The first element is the group name
    cout << "Number of fingertips: " << nBodyParts << endl;


    for (int i = 0; i < nBodyParts; i++)
    {

        fingertip_t fingertip;


        Bottle& subPart = rf.findGroup(bodyParts.get(i+1).asString());
        fingertip.fingerName = subPart.find("fingerName").asString();

        if (fingertip.fingerName.empty())
        {
            cerr << "No finger defined" << endl;
            return false;
        }

        fingertip.startIndex = subPart.check("startIndex", Value(-1)).asInt();
        if(fingertip.startIndex == -1)
        {

            cerr <<"Warning: no start index was defined using defalut value of 0" << endl;
            fingertip.startIndex = 0;
        }

        fingertip.modelFile_CoP = rf.findFileByName(subPart.find("model_CoP").asString());

        // Open it
        Property modelConf;
        modelConf.fromConfigFile(fingertip.modelFile_CoP);
        fingertip.model_CoP = new tacman::NeuralNet(modelConf);


        fingertip.modelFile_force = rf.findFileByName(subPart.find("model_force").asString());


        modelConf.clear();
        modelConf.fromConfigFile(fingertip.modelFile_force);
        fingertip.model_force = new tacman::NeuralNet(modelConf);


        // Open the port to publish CoP and force
        fingertip.dataPort = new BufferedPort<Bottle>();

        fingertip.dataPort->open("/" + moduleName + "/" + fingertip.fingerName);
        _fingertip_list.push_back(fingertip);

    }


    for (fingertipList_t::iterator it = _fingertip_list.begin(); it != _fingertip_list.end(); it++)
    {

        cout << "Finger Name: " << it->fingerName << endl;
        cout << "Starting index: " << it->startIndex << endl;
        cout << "Model file (CoP): " << it->modelFile_CoP << endl << endl;
        cout << "Model file (force): " << it->modelFile_force << endl << endl;
    }

    return true;
}

} // namespace tacman
