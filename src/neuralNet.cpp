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


        for ( unsigned int neuron = 0; neuron <= numNeurons; neuron++) // The extra neuron is for the bias
        {


          char buffer[256];
          sprintf(buffer, "neuron_%d", neuron);

          cout << buffer << endl;
            Bottle *weightsBottle = step.find(buffer).asList();

            if(weightsBottle != NULL)
            {
                vector<double> weights;

                for (int i = 0; i < weightsBottle->size(); i++)
                    weights.push_back(weightsBottle->get(i).asDouble());

                // std::cout << "Adding a neuron" << std::endl;
                _layers.back().push_back(Neuron(weights, neuron));
            }
            else
            {

                cout << "Warning: no connection weights defined" << endl;
                _layers.back().push_back(Neuron(1, neuron));

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
}

void NeuralNet::feedForward(vector<double> &input)
{
    assert(input.size() == _layers.at(0).size() - 1); // -1 for the bias node

    // Latch the input;
    for (unsigned int i = 0; i < input.size(); i++)
    {
        _layers[0][i].setOutputVal(input.at(i));
    }

    // Propogate forward
    for (unsigned int layerNum = 1; layerNum < _layers.size(); layerNum++)
    {

        Layer &previousLayer = _layers[layerNum - 1];


        for (unsigned int neuronNum = 0; neuronNum < _layers[layerNum].size()-1; neuronNum++)
        {
            _layers[layerNum][neuronNum].feedForward(previousLayer);
        }
    }

}

Neuron::Neuron(unsigned int numOutputs, unsigned int index)
{

    //std::cout << "Created a neuron!" << std::endl;
_myIndex = index;
    for(unsigned int c = 0; c < numOutputs; c++)
    {
        _outputWeights.push_back(connection(0.0, 0.0));

    }
}

Neuron::Neuron(vector<double> outputWeights, unsigned int index){

    //std::cout << "Created a neuron!";
    _myIndex = index;

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
        sum += previousLayer[n].getOutputVal() *
                previousLayer[n].getOutputWeight(_myIndex);
    }

    _outputVal = transferFunction(sum);
}

double Neuron::transferFunction(double val)
{
    return (1/(1+exp(-val)));
}

}
