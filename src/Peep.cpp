#include "Peep.h"

#include "Grid.h"
#include "AlgorithmHelpers.h"
#include "Parameters.h"
#include "PeepsPool.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>


//-------------------------------------------------------------------------
Peep::Peep(const Parameters& params, const Grid& grid)
    : m_Params(params)
    , m_Grid(grid)
{}

//-------------------------------------------------------------------------
void Peep::initialize(
    uint16_t index_,
    Coord loc_,
    Genetics::Genome &&genome_,
    RandomUintGenerator& random,
    Grid& grid)
{
    index = index_;
    loc = loc_;
    //birthLoc = loc_;
    grid.set(loc_, index_);
    age = 0;
    oscPeriod = 34; // ToDo !!! define a constant
    alive = true;
    lastMoveDir = Dir::random8(random);
    responsiveness = 0.5; // range 0.0..1.0
    longProbeDist = m_Params.longProbeDistance;
    challengeBits = (unsigned)false; // will be set true when some task gets accomplished
    genome = std::move(genome_);
    createWiringFromGenome();
}

//-------------------------------------------------------------------------
void Peep::makeRenumberedConnectionList(ConnectionList &connectionList, const Genetics::Genome &genome)
{
    connectionList.clear();
    for (auto const &gene : genome) {
        connectionList.push_back(gene);
        auto &conn = connectionList.back();

        if (conn.sourceType == Genetics::NEURON) {
            conn.sourceNum %= m_Params.maxNumberNeurons;
        } else {
            conn.sourceNum %= SensorsActions::Sensor::NUM_SENSES;
        }

        if (conn.sinkType == Genetics::NEURON) {
            conn.sinkNum %= m_Params.maxNumberNeurons;
        } else {
            conn.sinkNum %= SensorsActions::Action::NUM_ACTIONS;
        }
    }
}

//-------------------------------------------------------------------------
void Peep::makeNodeList(NodeMap &nodeMap, const ConnectionList &connectionList)
{
    nodeMap.clear();

    for (const auto &conn : connectionList) {
        if (conn.sinkType == Genetics::NEURON) {
            auto it = nodeMap.find(conn.sinkNum);
            if (it == nodeMap.end()) {
                assert(conn.sinkNum < m_Params.maxNumberNeurons);
                nodeMap.insert({conn.sinkNum, {}});
                it = nodeMap.find(conn.sinkNum);
                assert(it->first < m_Params.maxNumberNeurons);
                it->second.numOutputs = 0;
                it->second.numSelfInputs = 0;
                it->second.numInputsFromSensorsOrOtherNeurons = 0;
            }

            if (conn.sourceType == Genetics::NEURON && (conn.sourceNum == conn.sinkNum)) {
                ++(it->second.numSelfInputs);
            } else {
                ++(it->second.numInputsFromSensorsOrOtherNeurons);
            }
            assert(nodeMap.count(conn.sinkNum) == 1);
        }
        if (conn.sourceType == Genetics::NEURON) {
            auto it = nodeMap.find(conn.sourceNum);
            if (it == nodeMap.end()) {
                assert(conn.sourceNum < m_Params.maxNumberNeurons);
                nodeMap.insert({conn.sourceNum, {}});
                it = nodeMap.find(conn.sourceNum);
                assert(it->first < m_Params.maxNumberNeurons);
                it->second.numOutputs = 0;
                it->second.numSelfInputs = 0;
                it->second.numInputsFromSensorsOrOtherNeurons = 0;
            }
            ++(it->second.numOutputs);
            assert(nodeMap.count(conn.sourceNum) == 1);
        }
    }
}

//-------------------------------------------------------------------------
void Peep::removeConnectionsToNeuron(ConnectionList &connections, NodeMap &nodeMap, uint16_t neuronNumber)
{
    for (auto itConn = connections.begin(); itConn != connections.end(); ) {
        if (itConn->sinkType == Genetics::NEURON && itConn->sinkNum == neuronNumber) {
            // Remove the connection. If the connection source is from another
            // neuron, also decrement the other neuron's numOutputs:
            if (itConn->sourceType == Genetics::NEURON) {
                --(nodeMap[itConn->sourceNum].numOutputs);
            }
            itConn = connections.erase(itConn);
        } else {
            ++itConn;
        }
    }
}

//-------------------------------------------------------------------------
void Peep::cullUselessNeurons(ConnectionList &connections, NodeMap &nodeMap)
{
    bool allDone = false;
    while (!allDone) {
        allDone = true;
        for (auto itNeuron = nodeMap.begin(); itNeuron != nodeMap.end(); ) {
            assert(itNeuron->first < m_Params.maxNumberNeurons);
            // We're looking for neurons with zero outputs, or neurons that feed itself
            // and nobody else:
            if (itNeuron->second.numOutputs == itNeuron->second.numSelfInputs) {  // could be 0
                allDone = false;
                // Find and remove connections from sensors or other neurons
                removeConnectionsToNeuron(connections, nodeMap, itNeuron->first);
                itNeuron = nodeMap.erase(itNeuron);
            } else {
                ++itNeuron;
            }
        }
    }
}

//-------------------------------------------------------------------------
void Peep::createWiringFromGenome()
{
    NodeMap nodeMap;  // list of neurons and their number of inputs and outputs
    ConnectionList connectionList; // synaptic connections

    // Convert the indiv's genome to a renumbered connection list
    makeRenumberedConnectionList(connectionList, genome);

    // Make a node (neuron) list from the renumbered connection list
    makeNodeList(nodeMap, connectionList);

    // Find and remove neurons that don't feed anything or only feed themself.
    // This reiteratively removes all connections to the useless neurons.
    cullUselessNeurons(connectionList, nodeMap);

    // The neurons map now has all the referenced neurons, their neuron numbers, and
    // the number of outputs for each neuron. Now we'll renumber the neurons
    // starting at zero.

    assert(nodeMap.size() <= m_Params.maxNumberNeurons);
    uint16_t newNumber = 0;
    for (auto & node : nodeMap) {
        assert(node.second.numOutputs != 0);
        node.second.remappedNumber = newNumber++;
    }

    // Create the peep's connection list in two passes:
    // First the connections to neurons, then the connections to actions.
    // This ordering optimizes the feed-forward function in feedForward.cpp.

    nnet.connections.clear();

    // First, the connections from sensor or neuron to a neuron
    for (auto const &conn : connectionList) {
        if (conn.sinkType == Genetics::NEURON) {
            nnet.connections.push_back(conn);
            auto &newConn = nnet.connections.back();
            // fix the destination neuron number
            newConn.sinkNum = nodeMap[newConn.sinkNum].remappedNumber;
            // if the source is a neuron, fix its number too
            if (newConn.sourceType == Genetics::NEURON) {
                newConn.sourceNum = nodeMap[newConn.sourceNum].remappedNumber;
            }
        }
    }

    // Last, the connections from sensor or neuron to an action
    for (auto const &conn : connectionList) {
        if (conn.sinkType == Genetics::ACTION) {
            nnet.connections.push_back(conn);
            auto &newConn = nnet.connections.back();
            // if the source is a neuron, fix its number
            if (newConn.sourceType == Genetics::NEURON) {
                newConn.sourceNum = nodeMap[newConn.sourceNum].remappedNumber;
            }
        }
    }

    // Create the indiv's neural node list
    nnet.neurons.clear();
    for (unsigned neuronNum = 0; neuronNum < nodeMap.size(); ++neuronNum) {
        nnet.neurons.push_back( {} );
        nnet.neurons.back().output = Genetics::initialNeuronOutput();
        nnet.neurons.back().driven = (nodeMap[neuronNum].numInputsFromSensorsOrOtherNeurons != 0);
    }
}

//-------------------------------------------------------------------------
std::array<float, SensorsActions::Action::NUM_ACTIONS> Peep::feedForward(unsigned simStep, const PeepsPool& peeps, const PheromoneSignals& pheromoneSignals, RandomUintGenerator& random)
{
    // This container is used to return values for all the action outputs. This array
    // contains one value per action neuron, which is the sum of all its weighted
    // input connections. The sum has an arbitrary range. Return by value assumes compiler
    // return value optimization.
    std::array<float, SensorsActions::Action::NUM_ACTIONS> actionLevels;
    actionLevels.fill(0.0); // undriven actions default to value 0.0

    // Weighted inputs to each neuron are summed in neuronAccumulators[]
    std::vector<float> neuronAccumulators(nnet.neurons.size(), 0.0);

    // Connections were ordered at birth so that all connections to neurons get
    // processed here before any connections to actions. As soon as we encounter the
    // first connection to an action, we'll pass all the neuron input accumulators
    // through a transfer function and update the neuron outputs in the peep,
    // except for undriven neurons which act as bias feeds and don't change. The
    // transfer function will leave each neuron's output in the range -1.0..1.0.
    bool neuronOutputsComputed = false;
    for (auto& conn : nnet.connections) {
        if (conn.sinkType == Genetics::ACTION && !neuronOutputsComputed) {
            // We've handled all the connections from sensors and now we are about to
            // start on the connections to the action outputs, so now it's time to
            // update and latch all the neuron outputs to their proper range (-1.0..1.0)
            for (unsigned neuronIndex = 0; neuronIndex < nnet.neurons.size(); ++neuronIndex) {
                if (nnet.neurons[neuronIndex].driven) {
                    nnet.neurons[neuronIndex].output = std::tanh(neuronAccumulators[neuronIndex]);
                }
            }
            neuronOutputsComputed = true;
        }

        // Obtain the connection's input value from a sensor neuron or other neuron
        // The values are summed for now, later passed through a transfer function
        float inputVal;
        if (conn.sourceType == Genetics::SENSOR) {
            inputVal = SensorsActions::getSensor(*this, peeps, (SensorsActions::Sensor)conn.sourceNum, simStep, m_Grid, m_Params, random, pheromoneSignals);
        } else {
            inputVal = nnet.neurons[conn.sourceNum].output;
        }

        // Weight the connection's value and add to neuron accumulator or action accumulator.
        // The action and neuron accumulators will therefore contain +- float values in
        // an arbitrary range.
        if (conn.sinkType == Genetics::ACTION) {
            actionLevels[conn.sinkNum] += inputVal * conn.weightAsFloat();
        } else {
            neuronAccumulators[conn.sinkNum] += inputVal * conn.weightAsFloat();
        }
    }

    return actionLevels;
}

//-------------------------------------------------------------------------
void Peep::printGenome() const
{
    constexpr unsigned genesPerLine = 8;
    unsigned count = 0;
    for (Genetics::Gene gene : genome) {
        if (count == genesPerLine) {
            std::cout << std::endl;
            count = 0;
        } else if (count != 0) {
            std::cout << " ";
        }

        assert(sizeof(Genetics::Gene) == 4);
        uint32_t n;
        std::memcpy(&n, &gene, sizeof(n));
        std::cout << std::hex << std::setfill('0') << std::setw(8) << n;
        ++count;
    }
    std::cout << std::dec << std::endl;
}

//-------------------------------------------------------------------------
void Peep::printIGraphEdgeList() const
{
    for (auto & conn : nnet.connections) {
        if (conn.sourceType == Genetics::SENSOR) {
            std::cout << SensorsActions::sensorShortName((SensorsActions::Sensor)(conn.sourceNum));
        } else {
            std::cout << "N" << std::to_string(conn.sourceNum);
        }

        std::cout << " ";

        if (conn.sinkType == Genetics::ACTION) {
            std::cout << SensorsActions::actionShortName((SensorsActions::Action)(conn.sinkNum));
        } else {
            std::cout << "N" << std::to_string(conn.sinkNum);
        }

        std::cout << " " << std::to_string(conn.weight) << std::endl;
    }
}
