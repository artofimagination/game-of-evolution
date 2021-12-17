#include "RightHalf.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
RightHalf::RightHalf(const Parameters& params)
    : m_Params(params)
{
    m_Setup.topLeft = { (int16_t)(params.sizeX / 2.0), 0 };
    m_Setup.width = params.sizeX / 2.0;
    m_Setup.height = params.sizeY;
}

//-------------------------------------------------------------------------
std::pair<bool, float> RightHalf::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };

    return peep.loc.x > m_Setup.width / 2 ?
          std::pair<bool, float> { true, 1.0 }
        : std::pair<bool, float> { false, 0.0 };
};

} // namespace Challenges