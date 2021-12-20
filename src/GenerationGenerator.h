#pragma once

#include "Barriers/iBarriers.h"
#include "Challenges/iChallenges.h"
#include "Genome.h"
#include "PheromoneSignals.h"


class Grid;
class Parameters;
class Peep;
class PeepsPool;
class PheromoneSignals;
class RandomUintGenerator;

class GenerationGenerator
{

public:
    GenerationGenerator(
        Grid& grid,
        PeepsPool& peepsPool,
        PheromoneSignals& pheromones,
        const Parameters& params,
        RandomUintGenerator& random,
        const eBarrierType& barrierType,
        std::vector<std::unique_ptr<Barriers::iBarrier> >& barriers);

    // Requires that the grid, signals, and peeps containers have been allocated.
    // This will erase the grid and signal layers, then create a new population in
    // the peeps container at random locations with random genomes.
    void initializeGeneration0(eBarrierType barrierType);

    // At this point, the deferred death queue and move queue have been processed
    // and we are left with zero or more peeps who will repopulate the
    // world grid.
    // In order to redistribute the new population randomly, we will save all the
    // surviving genomes in a container, then clear the grid of indexes and generate
    // new peeps. This is inefficient when there are lots of survivors because
    // we could have reused (with mutations) the survivors' genomes and neural
    // nets instead of rebuilding them.
    // Returns number of survivor-reproducers.
    // Must be called in single-thread mode between generations.
    unsigned spawnNewGeneration(unsigned generation, unsigned murderCount);

    //! Creates a new challenge object if there is a challenge change.
    void SetChallenge(eChallenges challenge);
    //! Creates the first challenge object before the simulation starts.
    void SetStartChallenge(eChallenges challenge);

    //! Returns the current challenge id.
    eChallenges GetChallengeId() const { return m_CurrentChallenge; }

    //! Returns the current challenge.
    Challenges::iChallenge* GetChallenge() const { return m_xChallenge.get(); }

private:
    // Returns by value a single genome with random genes.
    Genetics::Genome makeRandomGenome();

    //! Requires a container with one or more parent genomes to choose from.
    //! Called from spawnNewGeneration(). This requires that the grid, signals, and
    //! peeps containers have been allocated. This will erase the grid and signal
    //! layers, then create a new population in the peeps container with random
    //! locations and genomes derived from the container of parent genomes.
    void initializeNewGeneration(
        const std::vector<Genetics::Genome> &parentGenomes,
        eBarrierType barrierType,
        unsigned generation);

    //! This generates a child genome from one or two parent genomes.
    //! If the parameter p.sexualReproduction is true, two parents contribute
    //! genes to the offspring. The new genome may undergo mutation.
    //! Must be called in single-thread mode between generations
    Genetics::Genome generateChildGenome(const std::vector<Genetics::Genome> &parentGenomes);

    //! The epoch log contains one line per generation in a format that can be.
    void appendEpochLog(unsigned generation, unsigned numberSurvivors, unsigned murderCount);

    Grid&                                               m_Grid;
    PeepsPool&                                          m_PeepsPool;
    PheromoneSignals&                                   m_PheromoneSignals;
    const Parameters&                                   m_Params;
    RandomUintGenerator&                                m_Random;
    std::unique_ptr<Challenges::iChallenge>             m_xChallenge{};
    eChallenges                                         m_CurrentChallenge{eChallenges::Altruism};
    const eBarrierType&                                 m_BarrierType;
    std::vector<std::unique_ptr<Barriers::iBarrier> >&  m_Barriers;
};