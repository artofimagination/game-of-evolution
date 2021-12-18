#include "EastWestEighths.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
EastWestEighths::EastWestEighths(const Parameters& params)
    : m_Params(params)
{
    m_Setup.eastTopLeft = { 0, 0 };
    m_Setup.westTopLeft = { static_cast<int16_t>(params.sizeX - params.sizeX / 8), 0 };
    m_Setup.width = params.sizeX / 8;
    m_Setup.height = params.sizeY;
}

//-------------------------------------------------------------------------
std::pair<bool, float> EastWestEighths::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    return peep.loc.x < m_Setup.width || peep.loc.x >= m_Setup.westTopLeft.x ?
          std::pair<bool, float> { true, 1.0 }
        : std::pair<bool, float> { false, 0.0 };
};

} // namespace Challenges