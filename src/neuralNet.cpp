#include "neuralNet.h"
#include <yarp/os/Bottle.h>
#include <cassert>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <stdlib.h>
#include <stdio.h>

namespace tacman
{

using std::cout;
using std::endl;
using yarp::os::Value;
using yarp::os::Bottle;

NeuralNet::NeuralNet(yarp::os::Property &modelConfig)
{
    // Read the topology from the config file
    //[Layers 01]

    std::cout << "NeuralNet" << std::endl;

    // Input/output mapping
    Bottle *inputXoffset = modelConfig.find("inputXoffset").asList();

    cout << "inputXoffset: [";
    if (inputXoffset != NULL)
    {
        for(int i = 0; i < inputXoffset->size(); i++)
        {
            _inputXoffset.push_back(inputXoffset->get(i).asDouble());
            cout << _inputXoffset.at(i) << " ";
        }
    }
    cout << "]" << endl;

    Bottle *inputGain = modelConfig.find("inputGain").asList();


    cout << "inputGain: [";
    if(inputGain != NULL)
    {
        for (int i = 0; i < inputGain->size(); i++)
        {
            _inputGain.push_back(inputGain->get(i).asDouble());
            cout << _inputGain.at(i) << " ";
        }
    }
    cout << "]" << endl;

    _inputYmin = modelConfig.check("inputYmin", Value(-1)).asDouble();

    cout << "inputYmin: " << _inputYmin << endl;


    Bottle *outputXoffset = modelConfig.find("outputXoffset").asList();

    cout << "oputputXoffset: [";
    if (outputXoffset != NULL)
    {
        for(int i = 0; i < outputXoffset->size(); i++)
        {
            _outputXoffset.push_back(outputXoffset->get(i).asDouble());
            cout << _outputXoffset.at(i) << " ";
        }
    }
    cout << "]" << endl;


    Bottle *outputGain = modelConfig.find("outputGain").asList();

    cout << "outputGain: [";
    if(outputGain != NULL)
    {
        for (int i = 0; i < outputGain->size(); i++)
        {
            _outputGain.push_back(outputGain->get(i).asDouble());
            cout << _outputGain.at(i) << " ";
        }
    }
    cout << "]" << endl;

    _outputYmin = modelConfig.check("outputYmin", Value(-1)).asDouble();

    cout << "outputYmin: " << _outputYmin << endl;
    //cout << _inputGain << endl;


    // Find the layer group
    Bottle &layers = modelConfig.findGroup("Layer");

    if(layers.isNull())
        cout << "No layers" << endl;




    unsigned int numLayers = layers.size();
    cout << "Number of layers: " << layers.size() -1 << endl;
    //unsigned int numInputs = 12;





    // The first entry is the Key, hence we start from 1
    for ( unsigned int numLayer = 1; numLayer < layers.size(); ++numLayer)
    {

        // Create a new layer and add it to the layers
        _layers.push_back(Layer());

        cout << "Layer: " << layers.get(numLayer).asString() << endl;

        Bottle& step = modelConfig.findGroup(layers.get(numLayer).asString());
        // Read the config file for the layer details
        if(step.isNull())
            cout << "Step is null" << endl;

        unsigned int numNeurons = step.check("numNeurons", Value(0)).asInt();
        cout << "Number of Neurons: " << numNeurons << endl;
        std::string transferFcn = step.check("transferFcn", Value("invalid")).asString();
        cout << "Transfer function: " << transferFcn << endl;

        for ( unsigned int neuron = 0; neuron <= numNeurons; neuron++) // The extra neuron is for the bias
        {


            char buffer[256];
            sprintf(buffer, "neuron_%d", neuron);

            //cout << buffer << endl;
            Bottle *weightsBottle = step.find(buffer).asList();

            if(weightsBottle != NULL)
            {
                vector<double> weights;

                for (int i = 0; i < weightsBottle->size(); i++)
                    weights.push_back(weightsBottle->get(i).asDouble());

                // std::cout << "Adding a neuron" << std::endl;
                _layers.back().push_back(Neuron(weights, neuron, transferFcn));
            }
            else
            {

                cout << "Warning: no connection weights defined" << endl;
                _layers.back().push_back(Neuron(1, neuron, transferFcn));

            }

        }

    }


}

void NeuralNet::getResults(vector<double> &results) const
{

    for(vector<Neuron>::const_iterator it = _layers.back().begin();
        it != (--_layers.back().end()); it ++) // The last neuron is the bias neuron
    {
        results.push_back(it->getOutputVal());
    }

    mapMinMax_reverse(results);
}

void NeuralNet::feedForward(vector<double> &input)
{
    assert(input.size() == _layers.at(0).size() - 1); // -1 for the bias node


    mapMinMax_apply(input);


    // Latch the input;
    //cout << "Layer 0:" << endl;
    for (unsigned int i = 0; i < input.size(); i++)
    {
        _layers[0][i].setOutputVal(input.at(i));
    }

    // Propogate forward
    for (unsigned int layerNum = 1; layerNum < _layers.size(); layerNum++)
    {

       // cout << "Layer: " << layerNum << endl;
        Layer &previousLayer = _layers[layerNum - 1];


        for (unsigned int neuronNum = 0; neuronNum < _layers[layerNum].size()-1; neuronNum++)
        {
            _layers[layerNum][neuronNum].feedForward(previousLayer);
        }
    }


}



Neuron::Neuron(unsigned int numOutputs, unsigned int index, std::string &transferFcn)
{
    cout << "TransferFcn:" << transferFcn << endl;

    //std::cout << "Created a neuron!" << std::endl;
    _myIndex = index;
     _outputVal = 1;
    if(transferFcn.compare("purelin") == 0)
        transferFunction = &transferFunction_purelin;
    else if(transferFcn.compare("tansig") == 0)
        transferFunction = &transferFunction_tansig;
    else
    {
        cout << "Warning, could not find " << transferFcn << endl;
        cout << "Using the default (purelin) transfer function\n" << endl;
    }
    for(unsigned int c = 0; c < numOutputs; c++)
    {
        _outputWeights.push_back(connection(0.0, 0.0));

    }
}

Neuron::Neuron(vector<double> outputWeights, unsigned int index, std::string &transferFcn){

    cout << "TransferFcn:" << transferFcn << endl;
    //std::cout << "Created a neuron!";
    _myIndex = index;
    _outputVal = 1;

    if(transferFcn.compare("purelin") == 0)
        transferFunction = &transferFunction_purelin;
    else if(transferFcn.compare("tansig") == 0)
        transferFunction = &transferFunction_tansig;
    else
    {
        cout << "Warning, could not find " << transferFcn << endl;
        cout << "Using the default (purelin) transfer function\n" << endl;
    }

    for(vector<double>::iterator it = outputWeights.begin(); it != outputWeights.end(); it++)
    {
        _outputWeights.push_back(connection(*it, 0.0)); // 0 is for delta weight
    }
}

void Neuron::feedForward(Layer &previousLayer)
{
    double sum = 0.0;

    // Sum the previous layer's outputs, including the bias

    for(unsigned int n = 0; n < previousLayer.size(); n++)
    {
        //cout << previousLayer[n].getOutputVal() << " * " << previousLayer[n].getOutputWeight(_myIndex) << endl;
        sum += previousLayer[n].getOutputVal() *
                previousLayer[n].getOutputWeight(_myIndex);
    }

    _outputVal = transferFunction(sum);
}

double Neuron::transferFunction_tansig(double val)
{
    //cout << "tansig: " << val << endl;
    // From the myNeuralNetwork function generated by matlab
    return (2.0 / ( 1.0 + exp(-2.0 * val)) - 1);
}

double Neuron::transferFunction_purelin(double val)
{
    //cout << "purelin: " << val << endl;
    return val;
}

void NeuralNet::mapMinMax_apply(vector<double> &inputVector) const
{
    // I am assuming everying is in order

    for (unsigned int i = 0; i < inputVector.size(); i++)
    {
        inputVector.at(i) = (inputVector.at(i) - _inputXoffset.at(i)) * _inputGain.at(i) + _inputYmin;
    }
}

void NeuralNet::mapMinMax_reverse(vector<double> &outputVector) const
{


    for (unsigned int i = 0; i < outputVector.size(); i++)

        outputVector.at(i) = (outputVector.at(i) - _outputYmin) / _outputGain.at(i) + _outputXoffset.at(i);
}

} // namespace tacman

