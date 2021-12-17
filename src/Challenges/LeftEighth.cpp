#include "LeftEighth.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
LeftEighth::LeftEighth(const Parameters& params)
    : m_Params(params)
{
    m_Setup.topLeft = { 0, 0 };
    m_Setup.width = params.sizeX / 8;
    m_Setup.height = params.sizeY;
}

//-------------------------------------------------------------------------
std::pair<bool, float> LeftEighth::PassedCriteria(const Peep& peep, const Parameters&, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    return peep.loc.x < m_Setup.width ?
          std::pair<bool, float> { true, 1.0 }
        : std::pair<bool, float> { false, 0.0 };
};

} // namespace Challenges