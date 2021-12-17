#include "AltruismSacrifice.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
AltruismSacrifice::AltruismSacrifice(const Parameters& params)
    : m_Params(params)
{
    m_Setup.center = {0, 0};
    m_Setup.radius = params.sizeX / 4.0; // in 128^2 world, holds 804 agents
}

//-------------------------------------------------------------------------
std::pair<bool, float> AltruismSacrifice::PassedCriteria(const Peep& peep, const Parameters& params, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };

    float distance = (Coord(params.sizeX - params.sizeX / 4, params.sizeY - params.sizeY / 4) - peep.loc).length();
    if (distance <= m_Setup.radius) {
        return { true, (m_Setup.radius - distance) / m_Setup.radius };
    } else {
        return { false, 0.0 };
    }
};

} // namespace Challenges