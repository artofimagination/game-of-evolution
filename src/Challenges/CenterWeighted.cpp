#include "CenterWeighted.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
CenterWeighted::CenterWeighted(const Parameters& params)
    : m_Params(params)
{
    m_Setup.safeCenter = { (int16_t)(params.sizeX / 2.0), (int16_t)(params.sizeY / 2.0) };
    m_Setup.radius = params.sizeX / 6.0;
}

//-------------------------------------------------------------------------
std::pair<bool, float> CenterWeighted::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };

    Coord offset = m_Setup.safeCenter - peep.loc;
    float distance = offset.length();
    return distance <= m_Setup.radius ?
          std::pair<bool, float> { true, 1.0 }
        : std::pair<bool, float> { false, 0.0 };
};

} // namespace Challenges