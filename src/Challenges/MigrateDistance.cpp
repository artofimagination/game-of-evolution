#include "MigrateDistance.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
MigrateDistance::MigrateDistance(const Parameters& params)
    : m_Params(params)
{

}

//-------------------------------------------------------------------------
std::pair<bool, float> MigrateDistance::PassedCriteria(const Peep& peep, const Parameters& params, const Grid&)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    float distance = (peep.loc - peep.birthLoc).length();
    distance = distance / (float)(std::max(params.sizeX, params.sizeY));
    return { true, distance };
};

} // namespace Challenges