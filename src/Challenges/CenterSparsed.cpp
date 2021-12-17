#include "CenterSparsed.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
CenterSparsed::CenterSparsed(const Parameters& params)
    : m_Params(params)
{
    m_Setup.center = { (int16_t)(params.sizeX / 2.0), (int16_t)(params.sizeY / 2.0) };
    m_Setup.radius = params.sizeX / 4.0;
    m_Setup.neighborRadius = 1.5;
    m_Setup.minNeighbors = 5;
    m_Setup.maxNeighbors = 8;
}

//-------------------------------------------------------------------------
std::pair<bool, float> CenterSparsed::PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid)
{
    if (!peep.alive)
        return { false, 0.0 };

    Coord offset = m_Setup.center - peep.loc;
    float distance = offset.length();
    if (distance <= m_Setup.radius) {
        unsigned count = 0;
        auto f = [&](Coord loc2){
            if (grid.isOccupiedAt(loc2)) ++count;
        };

        AlgorithmHelpers::visitNeighborhood(peep.loc, m_Setup.neighborRadius, f, params);
        if (count >= m_Setup.minNeighbors && count <= m_Setup.maxNeighbors) {
            return { true, 1.0 };
        }
    }
    return { false, 0.0 };
};

} // namespace Challenges