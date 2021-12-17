#include "GenerationGenerator.h"

#include "AlgorithmHelpers.h"
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

// //! Returns true and a score 0.0..1.0 if passed, false if failed.
// std::pair<bool, float> passedSurvivalCriterion(const Peep& peep, unsigned challenge, const Parameters& params, const Grid& grid)
// {
//     if (!peep.alive) {
//         return { false, 0.0 };
//     }

//     switch(challenge) {

//     // Survivors are those inside the circular area defined by
//     // safeCenter and radius
//     case CHALLENGE_CIRCLE:
//         {
//             Coord safeCenter { (int16_t)(params.sizeX / 4.0), (int16_t)(params.sizeY / 4.0) };
//             float radius = params.sizeX / 4.0;

//             Coord offset = safeCenter - peep.loc;
//             float distance = offset.length();
//             return distance <= radius ?
//                   std::pair<bool, float> { true, (radius - distance) / radius }
//                 : std::pair<bool, float> { false, 0.0 };
//         }

//     // Survivors are all those on the right side of the arena
//     case CHALLENGE_RIGHT_HALF:
//         return peep.loc.x > params.sizeX / 2 ?
//               std::pair<bool, float> { true, 1.0 }
//             : std::pair<bool, float> { false, 0.0 };

//     // Survivors are all those on the right quarter of the arena
//     case CHALLENGE_RIGHT_QUARTER:
//         return peep.loc.x > params.sizeX / 2 + params.sizeX / 4 ?
//               std::pair<bool, float> { true, 1.0 }
//             : std::pair<bool, float> { false, 0.0 };

//     // Survivors are all those on the left eighth of the arena
//     case CHALLENGE_LEFT_EIGHTH:
//         return peep.loc.x < params.sizeX / 8 ?
//               std::pair<bool, float> { true, 1.0 }
//             : std::pair<bool, float> { false, 0.0 };

//     // Survivors are those not touching the border and with exactly the number
//     // of neighbors defined by neighbors and radius, where neighbors includes self
//     case CHALLENGE_STRING:
//         {
//             unsigned minNeighbors = 22;
//             unsigned maxNeighbors = 2;
//             float radius = 1.5;

//             if (grid.isBorder(peep.loc)) {
//                 return { false, 0.0 };
//             }

//             unsigned count = 0;
//             auto f = [&](Coord loc2){
//                 if (grid.isOccupiedAt(loc2)) ++count;
//             };

//             AlgorithmHelpers::visitNeighborhood(peep.loc, radius, f, params);
//             if (count >= minNeighbors && count <= maxNeighbors) {
//                 return { true, 1.0 };
//             } else {
//                 return { false, 0.0 };
//             }
//         }

//     // Survivors are those within the specified radius of the center. The score
//     // is linearly weighted by distance from the center.
//     case CHALLENGE_CENTER_WEIGHTED:
//         {
//             Coord safeCenter { (int16_t)(params.sizeX / 2.0), (int16_t)(params.sizeY / 2.0) };
//             float radius = params.sizeX / 3.0;

//             Coord offset = safeCenter - peep.loc;
//             float distance = offset.length();
//             return distance <= radius ?
//                   std::pair<bool, float> { true, (radius - distance) / radius }
//                 : std::pair<bool, float> { false, 0.0 };
//         }

//     // Survivors are those within the specified radius of the center
//     case CHALLENGE_CENTER_UNWEIGHTED:
//         {
//             Coord safeCenter { (int16_t)(params.sizeX / 2.0), (int16_t)(params.sizeY / 2.0) };
//             float radius = params.sizeX / 3.0;

//             Coord offset = safeCenter - peep.loc;
//             float distance = offset.length();
//             return distance <= radius ?
//                   std::pair<bool, float> { true, 1.0 }
//                 : std::pair<bool, float> { false, 0.0 };
//         }

//     // Survivors are those within the specified outer radius of the center and with
//     // the specified number of neighbors in the specified inner radius.
//     // The score is not weighted by distance from the center.
//     case CHALLENGE_CENTER_SPARSE:
//         {
//             Coord safeCenter { (int16_t)(params.sizeX / 2.0), (int16_t)(params.sizeY / 2.0) };
//             float outerRadius = params.sizeX / 4.0;
//             float innerRadius = 1.5;
//             unsigned minNeighbors = 5;  // includes self
//             unsigned maxNeighbors = 8;

//             Coord offset = safeCenter - peep.loc;
//             float distance = offset.length();
//             if (distance <= outerRadius) {
//                 unsigned count = 0;
//                 auto f = [&](Coord loc2){
//                     if (grid.isOccupiedAt(loc2)) ++count;
//                 };

//                 AlgorithmHelpers::visitNeighborhood(peep.loc, innerRadius, f, params);
//                 if (count >= minNeighbors && count <= maxNeighbors) {
//                     return { true, 1.0 };
//                 }
//             }
//             return { false, 0.0 };
//         }

//     // Survivors are those within the specified radius of any corner.
//     // Assumes square arena.
//     case CHALLENGE_CORNER:
//         {
//             assert(params.sizeX == params.sizeY);
//             float radius = params.sizeX / 8.0;

//             float distance = (Coord(0, 0) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, 1.0 };
//             }
//             distance = (Coord(0, params.sizeY - 1) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, 1.0 };
//             }
//             distance = (Coord(params.sizeX - 1, 0) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, 1.0 };
//             }
//             distance = (Coord(params.sizeX - 1, params.sizeY - 1) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, 1.0 };
//             }
//             return { false, 0.0 };
//         }

//     // Survivors are those within the specified radius of any corner. The score
//     // is linearly weighted by distance from the corner point.
//     case CHALLENGE_CORNER_WEIGHTED:
//         {
//             assert(params.sizeX == params.sizeY);
//             float radius = params.sizeX / 4.0;

//             float distance = (Coord(0, 0) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, (radius - distance) / radius };
//             }
//             distance = (Coord(0, params.sizeY - 1) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, (radius - distance) / radius };
//             }
//             distance = (Coord(params.sizeX - 1, 0) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, (radius - distance) / radius };
//             }
//             distance = (Coord(params.sizeX - 1, params.sizeY - 1) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, (radius - distance) / radius };
//             }
//             return { false, 0.0 };
//         }

//     // This challenge is handled in endOfSimStep(), where peeps may die
//     // at the end of any sim step. There is nothing else to do here at the
//     // end of a generation. All remaining alive become parents.
//     case CHALLENGE_RADIOACTIVE_WALLS:
//         return { true, 1.0 };

//     // Survivors are those touching any wall at the end of the generation
//     case CHALLENGE_AGAINST_ANY_WALL:
//         {
//             bool onEdge = peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
//                        || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1;

//             if (onEdge) {
//                 return { true, 1.0 };
//             } else {
//                 return { false, 0.0 };
//             }
//         }

//     // This challenge is partially handled in endOfSimStep(), where peeps
//     // that are touching a wall are flagged in their Peep record. They are
//     // allowed to continue living. Here at the end of the generation, any that
//     // never touch a wall will die. All that touched a wall at any time during
//     // their life will become parents.
//     case CHALLENGE_TOUCH_ANY_WALL:
//         if (peep.challengeBits != 0) {
//             return { true, 1.0 };
//         } else {
//             return { false, 0.0 };
//         }

//     // Everybody survives and are candidate parents, but scored by how far
//     // they migrated from their birth location.
//     case CHALLENGE_MIGRATE_DISTANCE:
//         {
//             //unsigned requiredDistance = p.sizeX / 2.0;
//             float distance = (peep.loc - peep.birthLoc).length();
//             distance = distance / (float)(std::max(params.sizeX, params.sizeY));
//             return { true, distance };
//         }

//     // Survivors are all those on the left or right eighths of the arena
//     case CHALLENGE_EAST_WEST_EIGHTHS:
//         return peep.loc.x < params.sizeX / 8 || peep.loc.x >= (params.sizeX - params.sizeX / 8)?
//               std::pair<bool, float> { true, 1.0 }
//             : std::pair<bool, float> { false, 0.0 };

//     // Survivors are those within radius of any barrier center. Weighted by distance.
//     case CHALLENGE_NEAR_BARRIER:
//         {
//             float radius;
//             //radius = 20.0;
//             radius = params.sizeX / 2;
//             //radius = p.sizeX / 4;

//             const std::vector<Coord> barrierCenters = grid.getBarrierCenters();
//             float minDistance = 1e8;
//             for (Coord center : barrierCenters) {
//                 float distance = (peep.loc - center).length();
//                 if (distance < minDistance) {
//                     minDistance = distance;
//                 }
//             }
//             if (minDistance <= radius) {
//                 return { true, 1.0 - (minDistance / radius) };
//             } else {
//                 return { false, 0.0 };
//             }
//         }

//     // Survivors are those not touching a border and with exactly one neighbor which has no other neighbor
//     case CHALLENGE_PAIRS:
//         {
//             bool onEdge = peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
//                        || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1;

//             if (onEdge) {
//                 return { false, 0.0 };
//             }

//             unsigned count = 0;
//             for (int16_t x = peep.loc.x - 1; x < peep.loc.x + 1; ++x) {
//                 for (int16_t y = peep.loc.y - 1; y < peep.loc.y + 1; ++y) {
//                     Coord tloc = { x, y };
//                     if (tloc != peep.loc && grid.isInBounds(tloc) && grid.isOccupiedAt(tloc)) {
//                         ++count;
//                         if (count == 1) {
//                             for (int16_t x1 = tloc.x - 1; x1 < tloc.x + 1; ++x1) {
//                                 for (int16_t y1 = tloc.y - 1; y1 < tloc.y + 1; ++y1) {
//                                     Coord tloc1 = { x1, y1 };
//                                     if (tloc1 != tloc && tloc1 != peep.loc && grid.isInBounds(tloc1) && grid.isOccupiedAt(tloc1)) {
//                                         return { false, 0.0 };
//                                     }
//                                 }
//                             }
//                         } else {
//                             return { false, 0.0 };
//                         }
//                     }
//                 }
//             }
//             if (count == 1) {
//                 return { true, 1.0 };
//             } else {
//                 return { false, 0.0 };
//             }
//         }

//     // Survivors are those that contacted one or more specified locations in a sequence,
//     // ranked by the number of locations contacted. There will be a bit set in their
//     // challengeBits member for each location contacted.
//     case CHALLENGE_LOCATION_SEQUENCE:
//         {
//             unsigned count = 0;
//             unsigned bits = peep.challengeBits;
//             unsigned maxNumberOfBits = sizeof(bits) * 8;

//             for (unsigned n = 0; n < maxNumberOfBits; ++n) {
//                 if ((bits & (1 << n)) != 0) {
//                     ++count;
//                 }
//             }
//             if (count > 0) {
//                 return { true, count / (float)maxNumberOfBits };
//             } else {
//                 return { false, 0.0 };
//             }
//         }
//         break;

//     // Survivors are all those within the specified radius of the NE corner
//     case CHALLENGE_ALTRUISM_SACRIFICE:
//         {
//             //float radius = p.sizeX / 3.0; // in 128^2 world, holds 1429 agents
//             float radius = params.sizeX / 4.0; // in 128^2 world, holds 804 agents
//             //float radius = p.sizeX / 5.0; // in 128^2 world, holds 514 agents

//             float distance = (Coord(params.sizeX - params.sizeX / 4, params.sizeY - params.sizeY / 4) - peep.loc).length();
//             if (distance <= radius) {
//                 return { true, (radius - distance) / radius };
//             } else {
//                 return { false, 0.0 };
//             }
//         }

//     // Survivors are those inside the circular area defined by
//     // safeCenter and radius
//     case CHALLENGE_ALTRUISM:
//         {
//             Coord safeCenter { (int16_t)(params.sizeX / 4.0), (int16_t)(params.sizeY / 4.0) };
//             float radius = params.sizeX / 4.0; // in a 128^2 world, holds 3216

//             Coord offset = safeCenter - peep.loc;
//             float distance = offset.length();
//             return distance <= radius ?
//                   std::pair<bool, float> { true, (radius - distance) / radius }
//                 : std::pair<bool, float> { false, 0.0 };
//         }

//     default:
//         assert(false);
//     }
// }

//-------------------------------------------------------------------------
GenerationGenerator::GenerationGenerator(
    Grid& grid,
    PeepsPool& peepsPool,
    PheromoneSignals& pheromones,
    const Parameters& params,
    RandomUintGenerator& random)
    : m_Grid(grid)
    , m_PeepsPool(peepsPool)
    , m_PheromoneSignals(pheromones)
    , m_Params(params)
    , m_Random(random)
    , m_xChallenge(std::make_unique<Challenges::Altruism>(m_Random, m_Params))
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
void GenerationGenerator::initializeGeneration0()
{
    // The grid has already been allocated, just clear and reuse it
    m_Grid.zeroFill();
    m_Grid.createBarrier(m_Params.replaceBarrierTypeGenerationNumber == 0
                       ? m_Params.replaceBarrierType : m_Params.barrierType);

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

    if (g1.size() == 0 || g2.size() == 0) {
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
            assert(genome.size() > 0);
        } else {
            genome = g2;
            overlayWithSliceOf(g1);
            assert(genome.size() > 0);
        }

        // Trim to length = average length of parents
        unsigned sum = g1.size() + g2.size();
        // If average length is not an integral number, add one half the time
        if ((sum & 1) && (m_Random() & 1)) {
            ++sum;
        }
        Genetics::cropLength(genome, sum / 2, m_Random);
        assert(genome.size() > 0);
    } else {
        genome = g2;
        assert(genome.size() > 0);
    }

    Genetics::randomInsertDeletion(genome, m_Random, m_Params);
    assert(genome.size() > 0);
    applyPointMutations(genome, m_Random, m_Params);
    assert(genome.size() > 0);
    assert(genome.size() <= m_Params.genomeMaxLength);

    return genome;
}

//-------------------------------------------------------------------------
void GenerationGenerator::initializeNewGeneration(
    const std::vector<Genetics::Genome> &parentGenomes,
    unsigned generation)
{
    // The grid, signals, and peeps containers have already been allocated, just
    // clear them if needed and reuse the elements
    m_Grid.zeroFill();
    m_Grid.createBarrier(generation >= m_Params.replaceBarrierTypeGenerationNumber
                       ? m_Params.replaceBarrierType : m_Params.barrierType);
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
    // scores if the parents[] container was sorted by score
    for (const std::pair<uint16_t, float> &parent : parents) {
        parentGenomes.push_back(m_PeepsPool[parent.first].genome);
    }

    std::cout << "Gen " << generation << ", " << parentGenomes.size() << " survivors" << std::endl;
    appendEpochLog(generation, parentGenomes.size(), murderCount);
    //displaySignalUse(); // for debugging only

    // Now we have a container of zero or more parents' genomes

    if (parentGenomes.size() != 0) {
        // Spawn a new generation
        initializeNewGeneration(parentGenomes, generation + 1);
    } else {
        // Special case: there are no surviving parents: start the simulation over
        // from scratch with randomly-generated genomes
        initializeGeneration0();
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