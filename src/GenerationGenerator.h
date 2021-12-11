#pragma once

#include "Genome.h"
#include "PheromoneSignals.h"

class Grid;
class Parameters;
class Peep;
class PeepsPool;
class PheromoneSignals;
class RandomUintGenerator;

// Some of the survival challenges to try. Some are interesting, some
// not so much. Fine-tune the challenges by tweaking the corresponding code
// in survival-criteria.cpp.
constexpr unsigned CHALLENGE_CIRCLE = 0;
constexpr unsigned CHALLENGE_RIGHT_HALF = 1;
constexpr unsigned CHALLENGE_RIGHT_QUARTER = 2;
constexpr unsigned CHALLENGE_STRING = 3;
constexpr unsigned CHALLENGE_CENTER_WEIGHTED = 4;
constexpr unsigned CHALLENGE_CENTER_UNWEIGHTED = 40;
constexpr unsigned CHALLENGE_CORNER = 5;
constexpr unsigned CHALLENGE_CORNER_WEIGHTED = 6;
constexpr unsigned CHALLENGE_MIGRATE_DISTANCE = 7;
constexpr unsigned CHALLENGE_CENTER_SPARSE = 8;
constexpr unsigned CHALLENGE_LEFT_EIGHTH = 9;
constexpr unsigned CHALLENGE_RADIOACTIVE_WALLS = 10;
constexpr unsigned CHALLENGE_AGAINST_ANY_WALL = 11;
constexpr unsigned CHALLENGE_TOUCH_ANY_WALL = 12;
constexpr unsigned CHALLENGE_EAST_WEST_EIGHTHS = 13;
constexpr unsigned CHALLENGE_NEAR_BARRIER = 14;
constexpr unsigned CHALLENGE_PAIRS = 15;
constexpr unsigned CHALLENGE_LOCATION_SEQUENCE = 16;
constexpr unsigned CHALLENGE_ALTRUISM = 17;
constexpr unsigned CHALLENGE_ALTRUISM_SACRIFICE = 18;

class GenerationGenerator
{

public:
    GenerationGenerator(
        Grid& grid,
        PeepsPool& peepsPool,
        PheromoneSignals& pheromones,
        const Parameters& params,
        RandomUintGenerator& random);

    // Requires that the grid, signals, and peeps containers have been allocated.
    // This will erase the grid and signal layers, then create a new population in
    // the peeps container at random locations with random genomes.
    void initializeGeneration0();

    // At this point, the deferred death queue and move queue have been processed
    // and we are left with zero or more individuals who will repopulate the
    // world grid.
    // In order to redistribute the new population randomly, we will save all the
    // surviving genomes in a container, then clear the grid of indexes and generate
    // new individuals. This is inefficient when there are lots of survivors because
    // we could have reused (with mutations) the survivors' genomes and neural
    // nets instead of rebuilding them.
    // Returns number of survivor-reproducers.
    // Must be called in single-thread mode between generations.
    unsigned spawnNewGeneration(unsigned generation, unsigned murderCount);

private:
    // Returns by value a single genome with random genes.
    Genetics::Genome makeRandomGenome();

    //! Requires a container with one or more parent genomes to choose from.
    //! Called from spawnNewGeneration(). This requires that the grid, signals, and
    //! peeps containers have been allocated. This will erase the grid and signal
    //! layers, then create a new population in the peeps container with random
    //! locations and genomes derived from the container of parent genomes.
    void initializeNewGeneration(const std::vector<Genetics::Genome> &parentGenomes, unsigned generation);

    //! This generates a child genome from one or two parent genomes.
    //! If the parameter p.sexualReproduction is true, two parents contribute
    //! genes to the offspring. The new genome may undergo mutation.
    //! Must be called in single-thread mode between generations
    Genetics::Genome generateChildGenome(const std::vector<Genetics::Genome> &parentGenomes);

    //! The epoch log contains one line per generation in a format that can be.
    void appendEpochLog(unsigned generation, unsigned numberSurvivors, unsigned murderCount);

    Grid& m_Grid;
    PeepsPool& m_PeepsPool;
    PheromoneSignals& m_PheromoneSignals;
    const Parameters& m_Params;
    RandomUintGenerator& m_Random;
};