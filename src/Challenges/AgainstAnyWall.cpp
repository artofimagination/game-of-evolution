#include "AgainstAnyWall.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
AgainstAnyWall::AgainstAnyWall(const Parameters& params)
    : m_Params(params)
{
    m_Setup.borders = { 0, 1, 2, 3 };
}

//-------------------------------------------------------------------------
std::pair<bool, float> AgainstAnyWall::PassedCriteria(const Peep& peep, const Parameters& params, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    bool onEdge = peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
                || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1;

    if (onEdge) {
        return { true, 1.0 };
    } else {
        return { false, 0.0 };
    }
};

} // namespace Challenges