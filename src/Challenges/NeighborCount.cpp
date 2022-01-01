#include "NeighborCount.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
NeighborCount::NeighborCount(const Parameters& params)
    : m_Params(params)
{
    m_Setup.borders = { 0, 1, 2, 3 };
    m_Setup.minNeighbors = 5;
    m_Setup.maxNeighbors = 13;
    m_Setup.radius = 2.5;
}

//-------------------------------------------------------------------------
std::pair<bool, float> NeighborCount::PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid)
{
    if (!peep.alive)
        return { false, 0.0 };

    if (grid.isBorder(peep.loc)) {
        return { false, 0.0 };
    }

    unsigned count = 0;
    auto f = [&](Coord loc2){
        if (grid.isOccupiedAt(loc2)) ++count;
    };

    AlgorithmHelpers::visitNeighborhood(peep.loc, m_Setup.radius, f, params);
    if (count >= m_Setup.minNeighbors && count <= m_Setup.maxNeighbors) {
        return { true, 1.0 };
    } else {
        return { false, 0.0 };
    }
};

} // namespace Challenges