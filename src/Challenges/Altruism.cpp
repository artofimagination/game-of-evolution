#include "Altruism.h"

#include "AltruismSacrifice.h"
#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

#include <iostream>

namespace Challenges
{

//-------------------------------------------------------------------------
Altruism::Altruism(RandomUintGenerator& random, const Parameters& params)
    : m_Random(random)
    , m_Params(params)
{
    m_Setup.center = { (int16_t)(m_Params.sizeX / 4.0), (int16_t)(m_Params.sizeY / 4.0) };
    m_Setup.radius = m_Params.sizeX / 4.0; // in a 128^2 world, holds 3216
    AltruismSacrifice sacrificeChallenge(m_Params);
    m_Setup.sacrificeCenter = sacrificeChallenge.GetSetup().center;
    m_Setup.sacrificeRadius = sacrificeChallenge.GetSetup().radius;
}

//-------------------------------------------------------------------------
std::vector<std::pair<uint16_t, float> >& Altruism::EvaluateWhenNewGeneration(
    const PeepsPool& peeps,
    const Parameters& params,
    const Grid& grid,
    const Settings& settings)
{
    unsigned sacrificedCount = 0; // for the altruism challenge
    auto& parents = GetParents();

    // For the altruism challenge, test if the agent is inside either the sacrificial
    // or the spawning area. We'll count the number in the sacrificial area and
    // save the genomes of the ones in the spawning area, saving their scores
    // for later sorting. Indexes start at 1.
    bool considerKinship = true;
    std::vector<uint16_t> sacrificesIndexes; // those who gave their lives for the greater good

    for (uint16_t index = 1; index <= params.population; ++index) {
        // This the test for the spawning area:
        std::pair<bool, float> passed = PassedCriteria(peeps[index], params, grid);
        if (passed.first && !peeps[index].nnet.connections.empty()) {
            parents.push_back( { index, passed.second } );
        } else {
            // This is the test for the sacrificial area:
            AltruismSacrifice sacrificeChallenge(params);
            passed = sacrificeChallenge.PassedCriteria(peeps[index], params, grid);
            if (passed.first && !peeps[index].nnet.connections.empty()) {
                if (considerKinship) {
                    sacrificesIndexes.push_back(index);
                } else {
                    ++sacrificedCount;
                }
            }
        }
    }

    unsigned generationToApplyKinship = 10;
    constexpr unsigned altruismFactor = 10; // the saved:sacrificed ratio

    if (considerKinship) {
        if (settings.generation > generationToApplyKinship) {
            // Todo: optimize!!!
            float threshold = 0.7;

            std::vector<std::pair<uint16_t, float>> survivingKin;
            for (unsigned passes = 0; passes < altruismFactor; ++passes) {
                for (uint16_t sacrificedIndex : sacrificesIndexes) {
                    // randomize the next loop so we don't keep using the first one repeatedly
                    unsigned startIndex = m_Random(0, parents.size() - 1);
                    for (unsigned count = 0; count < parents.size(); ++count) {
                        const std::pair<uint16_t, float> &possibleParent = parents[(startIndex + count) % parents.size()];
                        const Genetics::Genome &g1 = peeps[sacrificedIndex].genome;
                        const Genetics::Genome &g2 = peeps[possibleParent.first].genome;
                        float similarity = Genetics::genomeSimilarity(g1, g2, params);
                        if (similarity >= threshold) {
                            survivingKin.push_back(possibleParent);
                            // mark this one so we don't use it again?
                            break;
                        }
                    }
                }
            }
            std::cout << parents.size() << " passed, "
                                        << sacrificesIndexes.size() << " sacrificed, "
                                        << survivingKin.size() << " saved" << std::endl; // !!!
            parents = std::move(survivingKin);
        }
    } else {
        // Limit the parent list
        unsigned numberSaved = sacrificedCount * altruismFactor;
        std::cout << parents.size() << " passed, " << sacrificedCount << " sacrificed, " << numberSaved << " saved" << std::endl; // !!!
        if (!parents.empty() && numberSaved < parents.size()) {
            parents.erase(parents.begin() + numberSaved, parents.end());
        }
    }
    return GetParents();
}

//-------------------------------------------------------------------------
std::pair<bool, float> Altruism::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };

    Coord offset = m_Setup.center - peep.loc;
    float distance = offset.length();
    return distance <= m_Setup.radius ?
          std::pair<bool, float> { true, (m_Setup.radius - distance) / m_Setup.radius }
        : std::pair<bool, float> { false, 0.0 };
};

} // namespace Challenges