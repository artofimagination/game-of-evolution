#pragma once

#include "Random.h"

#include <cstdint>
#include <vector>

class Parameters;
class PeepsPool;

namespace Genetics
{

constexpr uint8_t SENSOR = 1;  // always a source
constexpr uint8_t ACTION = 1;  // always a sink
constexpr uint8_t NEURON = 0;  // can be either a source or sink


//! Each gene specifies one synaptic connection in a neural net. Each
//! connection has an input (source) which is either a sensor or another neuron.
//! Each connection has an output, which is either an action or another neuron.
//! Each connection has a floating point weight derived from a signed 16-bit
//! value. The signed integer weight is scaled to a small range, then cubed
//! to provide fine resolution near zero.
struct __attribute__((packed)) Gene 
{

    float weightAsFloat() const { return weight / 8192.0; }
    static int16_t makeRandomWeight(RandomUintGenerator& r) { return r(0, 0xefff) - 0x8000; }

    uint16_t sourceType:1; // SENSOR or NEURON
    uint16_t sourceNum:7;
    uint16_t sinkType:1;   // NEURON or ACTION
    uint16_t sinkNum:7;
    int16_t weight;
};

// An individual's genome is a set of Genes (see Gene comments above). Each
// gene is equivalent to one connection in a neural net. An individual's
// neural net is derived from its set of genes.
typedef std::vector<Gene> Genome;

// An individual's "brain" is a neural net specified by a set
// of Genes where each Gene specifies one connection in the neural net (see
// Genome comments above). Each neuron has a single output which is
// connected to a set of sinks where each sink is either an action output
// or another neuron. Each neuron has a set of input sources where each
// source is either a sensor or another neuron. There is no concept of
// layers in the net: it's a free-for-all topology with forward, backwards,
// and sideways connection allowed. Weighted connections are allowed
// directly from any source to any action.

// Currently the genome does not specify the activation function used in
// the neurons. (May be hardcoded to std::tanh() !!!)

// When the input is a sensor, the input value to the sink is the raw
// sensor value of type float and depends on the sensor. If the output
// is an action, the source's output value is interpreted by the action
// node and whether the action occurs or not depends on the action's
// implementation.

// In the genome, neurons are identified by 15-bit unsigned indices,
// which are reinterpreted as values in the range 0..p.genomeMaxLength-1
// by taking the 15-bit index modulo the max number of allowed neurons.
// In the neural net, the neurons that end up connected get new indices
// assigned sequentially starting at 0.
struct NeuralNet {
    std::vector<Gene> connections; // connections are equivalent to genes

    struct Neuron {
        float output;
        bool driven;        // undriven neurons have fixed output values
    };
    std::vector<Neuron> neurons;
};

// When a new population is generated and every individual is given a
// neural net, the neuron outputs must be initialized to something:
//constexpr float initialNeuronOutput() { return (NEURON_RANGE / 2.0) + NEURON_MIN; }
constexpr float initialNeuronOutput() { return 0.5; }

//! Returns 0.0..1.0
//!
//! ToDo: optimize by approximation for long genomes
float genomeSimilarity(const Genome& g1, const Genome& g2, const Parameters& params);

void displaySampleGenomes(unsigned count, const PeepsPool& peeps, const Parameters& params);

float averageGenomeLength(const PeepsPool& peeps, RandomUintGenerator& random, const Parameters& params);

//! returns 0.0..1.0
//! Samples random pairs of peeps regardless if they are alive or not
float geneticDiversity(const PeepsPool& peeps, RandomUintGenerator& random, const Parameters& params);

//! If the genome is longer than the prescribed length, and if it's longer
//! than one gene, then we remove genes from the front or back. This is
//! used only when the simulator is configured to allow genomes of
//! unequal lengths during a simulation.
void cropLength(Genome &genome, unsigned length, RandomUintGenerator& random);

//! This applies a point mutation at a random bit in a genome.
void randomBitFlip(Genome &genome, RandomUintGenerator& random);

//! Inserts or removes a single gene from the genome. This is
//! used only when the simulator is configured to allow genomes of
//! unequal lengths during a simulation.
void randomInsertDeletion(Genome &genome, RandomUintGenerator& random, const Parameters& params);

//! This function causes point mutations in a genome with a probability defined
//! by the parameter p.pointMutationRate.
void applyPointMutations(Genome &genome, RandomUintGenerator& random, const Parameters& params);

//! Returns by value a single gene with random members.
//! See genome.h for the width of the members.
//! ToDo: don't assume the width of the members in gene.
Genetics::Gene makeRandomGene(RandomUintGenerator& random);

uint8_t makeGeneticColor(const Genome &genome);

} // namespace Genetics