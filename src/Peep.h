#pragma once

#include "BasicTypes.h"
#include "Genome.h"
#include "SensorsActions.h"

#include <array>
#include <cstdint>
#include <list>
#include <map>

class Grid;
class Parameters;
class RandomUintGenerator;

class Peep 
{
public:
    Peep(const Parameters& params, const Grid& grid);


    /********************************************************************************
    This function does a neural net feed-forward operation, from sensor (input) neurons
    through internal neurons to action (output) neurons. The feed-forward
    calculations are evaluated once each simulator step (simStep).

    There is no back-propagation in this simulator. Once an individual's neural net
    brain is wired at birth, the weights and topology do not change during the
    individual's lifetime.

    The data structure Indiv::neurons contains internal neurons, and Indiv::connections
    holds the connections between the neurons.

    We have three types of neurons:

        input sensors - each gives a value in the range SENSOR_MIN.. SENSOR_MAX (0.0..1.0).
            Values are obtained from getSensor().

        internal neurons - each takes inputs from sensors or other internal neurons;
            each has output value in the range NEURON_MIN..NEURON_MAX (-1.0..1.0). The
            output value for each neuron is stored in Indiv::neurons[] and survives from
            one simStep to the next. (For example, a neuron that feeds itself will use
            its output value that was latched from the previous simStep.) Inputs to the
            neurons are summed each simStep in a temporary container and then discarded
            after the neurons' outputs are computed.

        action (output) neurons - each takes inputs from sensors or other internal
            neurons; In this function, each has an output value in an arbitrary range
            (because they are the raw sums of zero or more weighted inputs).
            The values of the action neurons are saved in local container
            actionLevels[] which is returned to the caller by value (thanks RVO).
    ********************************************************************************/
    std::array<float, SensorsActions::Action::NUM_ACTIONS> feedForward(unsigned simStep, const PeepsPool& peeps, const PheromoneSignals& pheromoneSignals, RandomUintGenerator& random); // reads sensors, returns actions

    //! This is called when any individual is spawned.
    //! The responsiveness parameter will be initialized here to maximum value
    //! of 1.0, then depending on which action activation function is used,
    //! the default undriven value may be changed to 1.0 or action midrange.
    void initialize(
        uint16_t index_,
        Coord loc_,
        Genetics::Genome &&genome_,
        RandomUintGenerator& random,
        Grid& grid);
    
    //! This function is used when an agent is spawned. This function converts the
    //! agent's inherited genome into the agent's neural net brain. There is a close
    //! correspondence between the genome and the neural net, but a connection
    //! specified in the genome will not be represented in the neural net if the
    //! connection feeds a neuron that does not itself feed anything else.
    //! Neurons get renumbered in the process:
    //! 1. Create a set of referenced neuron numbers where each index is in the
    //!    range 0..p.genomeMaxLength-1, keeping a count of outputs for each neuron.
    //! 2. Delete any referenced neuron index that has no outputs or only feeds itself.
    //! 3. Renumber the remaining neurons sequentially starting at 0.
    void createWiringFromGenome(); // creates .nnet member from .genome member
    // void printNeuralNet() const;
    //! This prints a neural net in a form that can be processed with
    //! graph-nnet.py to produce a graphic illustration of the net.
    void printIGraphEdgeList() const;
    //! Format: 32-bit hex strings, one per gene.
    void printGenome() const;

    bool alive;
    uint16_t index;             //!< index into PeepsPool[] container
    Coord loc;                  //!< refers to a location in grid[][]
    Coord birthLoc;
    unsigned age;
    Genetics::Genome genome;
    Genetics::NeuralNet nnet;   //!< derived from .genome
    float responsiveness;       //!< 0.0..1.0 (0 is like asleep)
    unsigned oscPeriod;         //!< 2..4*p.stepsPerGeneration (TBD, see executeActions())
    unsigned longProbeDist;     //!< distance for long forward probe for obstructions
    Dir lastMoveDir;            //!< direction of last movement
    unsigned challengeBits;     //!< modified when the peep accomplishes some task
private:
    //! This structure is used while converting the connection list to a
    //! neural net. This helps us to find neurons that don't feed anything
    //! so that they can be removed along with all the connections that
    //! feed the useless neurons. We'll cull neurons with .numOutputs == 0
    //! or those that only feed themselves, i.e., .numSelfInputs == .numOutputs.
    //! Finally, we'll renumber the remaining neurons sequentially starting
    //! at zero using the .remappedNumber member.
    struct Node {
        uint16_t remappedNumber;
        uint16_t numOutputs;
        uint16_t numSelfInputs;
        uint16_t numInputsFromSensorsOrOtherNeurons;
    };

    //! Two neuron renumberings occur: The original genome uses a uint16_t for
    //! neuron numbers. The first renumbering maps 16-bit unsigned neuron numbers
    //! to the range 0..p.maxNumberNeurons - 1. After culling useless neurons
    //! (see comments above), we'll renumber the remaining neurons sequentially
    //! starting at 0.
    typedef std::map<uint16_t, Node> NodeMap; // key is neuron number 0..p.maxNumberNeurons - 1

    typedef std::list<Genetics::Gene> ConnectionList;
    //! Convert the indiv's genome to a renumbered connection list.
    //! This renumbers the neurons from their uint16_t values in the genome
    //! to the range 0..p.maxNumberNeurons - 1 by using a modulo operator.
    //! Sensors are renumbered 0..Sensor::NUM_SENSES - 1
    //! Actions are renumbered 0..Action::NUM_ACTIONS - 1
    void makeRenumberedConnectionList(ConnectionList &connectionList, const Genetics::Genome &genome);

    //! Scan the connections and make a list of all the neuron numbers
    //! mentioned in the connections. Also keep track of how many inputs and
    //! outputs each neuron has.
    void makeNodeList(NodeMap &nodeMap, const ConnectionList &connectionList);

    // If a neuron has no outputs or only outputs that feed itself, then we
    // remove it along with all connections that feed it. Reiterative, because
    // after we remove a connection to a useless neuron, it may result in a
    // different neuron having no outputs.
    void cullUselessNeurons(ConnectionList &connections, NodeMap &nodeMap);

    // During the culling process, we will remove any neuron that has no outputs,
    // and all the connections that feed the useless neuron.
    void removeConnectionsToNeuron(ConnectionList &connections, NodeMap &nodeMap, uint16_t neuronNumber);

private:
    const Parameters& m_Params;
    const Grid& m_Grid;
};