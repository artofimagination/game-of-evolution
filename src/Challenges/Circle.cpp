#include "Circle.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
Circle::Circle(const Parameters& params)
    : m_Params(params)
{
    m_Setup.center = { (int16_t)(params.sizeX / 4.0), (int16_t)(params.sizeY / 4.0) };
    m_Setup.radius = params.sizeX / 4.0;
}

//-------------------------------------------------------------------------
std::pair<bool, float> Circle::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
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