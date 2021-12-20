#include "GenerationGenerator.h"

#include "AlgorithmHelpers.h"
#include "Barriers/iBarriers.h"
#include "Challenges/Altruism.h"
#include "Challenges/iChallenges.h"
#include "Grid.h"
#include "Parameters.h"
#include "PeepsPool.h"
#include "PheromoneSignals.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>


//-------------------------------------------------------------------------
GenerationGenerator::GenerationGenerator(
    Grid& grid,
    PeepsPool& peepsPool,
    PheromoneSignals& pheromones,
    const Parameters& params,
    RandomUintGenerator& random,
    const eBarrierType& barrierType,
    std::vector<std::unique_ptr<Barriers::iBarrier> >& barriers)
    : m_Grid(grid)
    , m_PeepsPool(peepsPool)
    , m_PheromoneSignals(pheromones)
    , m_Params(params)
    , m_Random(random)
    , m_xChallenge(std::make_unique<Challenges::Altruism>(m_Random, m_Params))
    , m_BarrierType(barrierType)
    , m_Barriers(barriers)
{
    
}

//-------------------------------------------------------------------------
void GenerationGenerator::SetChallenge(eChallenges challenge)
{
    if(m_CurrentChallenge != challenge)
    {
        m_xChallenge = std::unique_ptr<Challenges::iChallenge>(Challenges::CreateChallenge(challenge, m_Random, m_Params));
        m_CurrentChallenge = challenge;
    }
}

//-------------------------------------------------------------------------
void GenerationGenerator::SetStartChallenge(eChallenges challenge)
{
    m_xChallenge = std::unique_ptr<Challenges::iChallenge>(Challenges::CreateChallenge(challenge, m_Random, m_Params));
    m_CurrentChallenge = challenge;
}

//-------------------------------------------------------------------------
void GenerationGenerator::initializeGeneration0(eBarrierType barrierType)
{
    // The grid has already been allocated, just clear and reuse it
    m_Grid.zeroFill();
    m_Grid.createBarrier(m_Params.replaceBarrierTypeGenerationNumber == 0
                       ? static_cast<eBarrierType>(m_Params.replaceBarrierType) : 
                       static_cast<eBarrierType>(barrierType), m_Barriers);

    // The signal layers have already been allocated, so just reuse them
    m_PheromoneSignals.zeroFill();

    // Spawn the population. The peeps container has already been allocated,
    // just clear and reuse it
    for (uint16_t index = 1; index <= m_Params.population; ++index) {
        m_PeepsPool[index].initialize(index, m_Grid.findEmptyLocation(), makeRandomGenome(), m_Random, m_Grid);
    }
}

//-------------------------------------------------------------------------
Genetics::Genome GenerationGenerator::makeRandomGenome()
{
    Genetics::Genome genome;

    unsigned length = m_Random(m_Params.genomeInitialLengthMin, m_Params.genomeInitialLengthMax);
    for (unsigned n = 0; n < length; ++n) {
        genome.push_back(Genetics::makeRandomGene(m_Random));
    }

    return genome;
}

//-------------------------------------------------------------------------
Genetics::Genome GenerationGenerator::generateChildGenome(const std::vector<Genetics::Genome> &parentGenomes)
{
    // random parent (or parents if sexual reproduction) with random
    // mutations
    Genetics::Genome genome;

    uint16_t parent1Idx;
    uint16_t parent2Idx;

    // Choose two parents randomly from the candidates. If the parameter
    // p.chooseParentsByFitness is false, then we choose at random from
    // all the candidate parents with equal preference. If the parameter is
    // true, then we give preference to candidate parents according to their
    // score. Their score was computed by the survival/selection algorithm
    // in survival-criteria.cpp.
    if (m_Params.chooseParentsByFitness && parentGenomes.size() > 1) {
        parent1Idx = m_Random(1, parentGenomes.size() - 1);
        parent2Idx = m_Random(0, parent1Idx - 1);
    } else {
        parent1Idx = m_Random(0, parentGenomes.size() - 1);
        parent2Idx = m_Random(0, parentGenomes.size() - 1);
    }

    const Genetics::Genome &g1 = parentGenomes[parent1Idx];
    const Genetics::Genome &g2 = parentGenomes[parent2Idx];

    if (g1.empty() || g2.empty()) {
        std::cout << "invalid genome" << std::endl;
        assert(false);
    }

    auto overlayWithSliceOf = [&](const Genetics::Genome &gShorter) {
        uint16_t index0 = m_Random(0, gShorter.size() - 1);
        uint16_t index1 = m_Random(0, gShorter.size());
        if (index0 > index1) {
            std::swap(index0, index1);
        }
        std::copy(&gShorter[index0], &gShorter[index1], &genome[index0]);
    };

    if (m_Params.sexualReproduction) {
        if (g1.size() > g2.size()) {
            genome = g1;
            overlayWithSliceOf(g2);
            assert(!genome.empty());
        } else {
            genome = g2;
            overlayWithSliceOf(g1);
            assert(!genome.empty());
        }

        // Trim to length = average length of parents
        unsigned sum = g1.size() + g2.size();
        // If average length is not an integral number, add one half the time
        if ((sum & 1) && (m_Random() & 1)) {
            ++sum;
        }
        Genetics::cropLength(genome, sum / 2, m_Random);
        assert(!genome.empty());
    } else {
        genome = g2;
        assert(!genome.empty());
    }

    Genetics::randomInsertDeletion(genome, m_Random, m_Params);
    assert(!genome.empty());
    applyPointMutations(genome, m_Random, m_Params);
    assert(!genome.empty());
    assert(genome.size() <= m_Params.genomeMaxLength);

    return genome;
}

//-------------------------------------------------------------------------
void GenerationGenerator::initializeNewGeneration(
    const std::vector<Genetics::Genome> &parentGenomes,
    eBarrierType barrierType,
    unsigned generation)
{
    // The grid, signals, and peeps containers have already been allocated, just
    // clear them if needed and reuse the elements
    m_Grid.zeroFill();
    m_Grid.createBarrier(generation >= m_Params.replaceBarrierTypeGenerationNumber
                       ? static_cast<eBarrierType>(m_Params.replaceBarrierType) : 
                       static_cast<eBarrierType>(barrierType),
                       m_Barriers);
    m_PheromoneSignals.zeroFill();

    // Spawn the population. This overwrites all the elements of peeps[]
    for (uint16_t index = 1; index <= m_Params.population; ++index) {
        m_PeepsPool[index].initialize(index, m_Grid.findEmptyLocation(), generateChildGenome(parentGenomes), m_Random, m_Grid);
    }
}

//-------------------------------------------------------------------------
unsigned GenerationGenerator::spawnNewGeneration(unsigned generation, unsigned murderCount)
{
    auto settings = Challenges::Settings();
    settings.generation = generation;
    settings.murderCount = murderCount;
    // This container will hold the genomes of the survivors
    std::vector<Genetics::Genome> parentGenomes;
    // This container will hold the indexes and survival scores (0.0..1.0)
    // of all the survivors who will provide genomes for repopulation.
    std::vector<std::pair<uint16_t, float>> parents = m_xChallenge->EvaluateWhenNewGeneration(
        m_PeepsPool,
        m_Params,
        m_Grid,
        settings);

    // Sort the indexes of the parents by their fitness scores
    std::sort(parents.begin(), parents.end(),
        [](const std::pair<uint16_t, float> &parent1, const std::pair<uint16_t, float> &parent2) {
            return parent1.second > parent2.second;
        });

    // Assemble a list of all the parent genomes. These will be ordered by their
    // scores if the parents[] container was sorted by score.
    parentGenomes.reserve(parents.size());
    for (const std::pair<uint16_t, float> &parent : parents) {
        parentGenomes.push_back(m_PeepsPool[parent.first].genome);
    }

    std::cout << "Gen " << generation << ", " << parentGenomes.size() << " survivors" << std::endl;
    appendEpochLog(generation, parentGenomes.size(), murderCount);
    //displaySignalUse(); // for debugging only

    // Now we have a container of zero or more parents' genomes

    if (!parentGenomes.empty()) {
        // Spawn a new generation
        initializeNewGeneration(parentGenomes, m_BarrierType, generation + 1);
    } else {
        // Special case: there are no surviving parents: start the simulation over
        // from scratch with randomly-generated genomes
        initializeGeneration0(m_BarrierType);
    }

    return parentGenomes.size();
}

// The epoch log contains one line per generation in a format that can be.
void GenerationGenerator::appendEpochLog(unsigned generation, unsigned numberSurvivors, unsigned murderCount)
{
    std::ofstream foutput;

    if (generation == 0) {
        foutput.open(m_Params.logDir + "/epoch-log.txt");
        foutput.close();
    }

    foutput.open(m_Params.logDir + "/epoch-log.txt", std::ios::app);

    if (foutput.is_open()) {
        foutput << generation << " " << numberSurvivors << " " << Genetics::geneticDiversity(m_PeepsPool, m_Random, m_Params)
                << " " << Genetics::averageGenomeLength(m_PeepsPool, m_Random, m_Params) << " " << murderCount << std::endl;
    } else {
        assert(false);
    }
}