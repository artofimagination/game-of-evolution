#include "Corner.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

#include <cassert>

namespace Challenges
{

//-------------------------------------------------------------------------
Corner::Corner(const Parameters& params)
    : m_Params(params)
{
    m_Setup.centers.emplace_back(Coord{ 0, 0 });
    m_Setup.centers.emplace_back(Coord{ 0, (int16_t)(params.sizeY - 1) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(params.sizeX - 1), 0 });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(params.sizeX - 1), (int16_t)(params.sizeY - 1) });
    m_Setup.radius = params.sizeX / 8.0;

}

//-------------------------------------------------------------------------
std::pair<bool, float> Corner::PassedCriteria(const Peep& peep, const Parameters& params, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };

    assert(params.sizeX == params.sizeY);

    float distance = (m_Setup.centers[0] - peep.loc).length();
    if (distance <= m_Setup.radius) {
        return { true, 1.0 };
    }
    distance = (m_Setup.centers[1] - peep.loc).length();
    if (distance <= m_Setup.radius) {
        return { true, 1.0 };
    }
    distance = (m_Setup.centers[2] - peep.loc).length();
    if (distance <= m_Setup.radius) {
        return { true, 1.0 };
    }
    distance = (m_Setup.centers[3] - peep.loc).length();
    if (distance <= m_Setup.radius) {
        return { true, 1.0 };
    }
    return { false, 0.0 };
};

} // namespace Challenges