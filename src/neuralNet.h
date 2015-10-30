#pragma once
#include <yarp/os/ResourceFinder.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <yarp/os/Property.h>

namespace tacman
{

using yarp::os::ResourceFinder;
using std::vector;
using yarp::os::Property;

class Neuron;

typedef vector<Neuron> Layer;

class NeuralNet
{
public:
    NeuralNet(Property &modelConfig);
    void feedForward(vector<double> input);
    void getResults(vector<double> &results) const;


private:
    // Neural net is a vector of layers
    vector<Layer> _layers; //_layers[layerNum][neuronNum]
    // The layers are connected

    // The input transfer data
private:

    vector<double> _inputXoffset;
    vector<double> _inputGain;
    double _inputYmin;

    vector<double> _outputXoffset;
    vector<double> _outputGain;
    double _outputYmin;

    void mapMinMax_apply(vector<double> &inputVector) const;
    void mapMinMax_reverse(vector<double> &outputVector) const;

};

struct connection
{

    connection(double wt, double deltaWt): weight(wt), deltaWeight(deltaWt){}
    double weight;
    double deltaWeight;
};

class Neuron
{
public:
    Neuron(unsigned int numOutputs, unsigned int index, std::string &transferFcn);
    Neuron(vector<double> outputWeights, unsigned int index, std::string &transferFcn);


    void feedForward(Layer &previousLayer);

    void setOutputVal(double &input){ _outputVal = input;}
    double getOutputVal(void) const {return _outputVal;}
    double getOutputWeight(unsigned int index){return _outputWeights.at(index).weight;}

private:
    double _outputVal;
    vector<connection> _outputWeights;
    unsigned int _myIndex;
    double (*transferFunction)(double);

    static double transferFunction_tansig(double val);
    static double transferFunction_purelin(double val);



};
}
