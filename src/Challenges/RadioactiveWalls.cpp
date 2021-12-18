#include "RadioactiveWalls.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

namespace Challenges
{

//-------------------------------------------------------------------------
RadioactiveWalls::RadioactiveWalls(const Parameters& params, RandomUintGenerator& random)
    : m_Params(params)
    , m_Random(random)
{
    m_Setup.border = 0;
    m_Setup.distance = params.sizeX / 2;
}

//-------------------------------------------------------------------------
std::pair<bool, float> RadioactiveWalls::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{ 
    if (!peep.alive)
        return { false, 0.0 };

    return {true, 1.0};
};

//-------------------------------------------------------------------------
void RadioactiveWalls::EvaluateAtEndOfSimStep(
    PeepsPool& peeps,
    const Parameters& params,
    const Grid&,
    const Settings& settings)
{
    auto condition = (settings.simStep < params.stepsPerGeneration / 2);
    int16_t radioactiveX =  condition ? 0 : params.sizeX - 1;
    m_Setup.border = condition ? 0 : 2;
    for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
        Peep &peep = peeps[index];
        int16_t distanceFromRadioactiveWall = std::abs(peep.loc.x - radioactiveX);
        if (distanceFromRadioactiveWall < static_cast<int16_t>(m_Setup.distance)) {
            float chanceOfDeath = 1.0 / distanceFromRadioactiveWall;
            if (m_Random() / (float)RANDOM_UINT_MAX < chanceOfDeath) {
                peeps.queueForDeath(peep);
            }
        }
    }
}

} // namespace Challenges