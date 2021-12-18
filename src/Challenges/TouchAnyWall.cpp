#include "TouchAnyWall.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

namespace Challenges
{

//-------------------------------------------------------------------------
TouchAnyWall::TouchAnyWall(const Parameters& params)
    : m_Params(params)
{
    m_Setup.borders = { 0, 1, 2, 3 };
}

//-------------------------------------------------------------------------
std::pair<bool, float> TouchAnyWall::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    if (peep.challengeBits != 0) {
        return { true, 1.0 };
    } else {
        return { false, 0.0 };
    }
};

//-------------------------------------------------------------------------
void TouchAnyWall::EvaluateAtEndOfSimStep(
    PeepsPool& peeps,
    const Parameters& params,
    const Grid&,
    const Settings&)
{
    for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
        Peep &peep = peeps[index];
        if (peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
          || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1) {
            peep.challengeBits = true;
        }
    }
}

} // namespace Challenges