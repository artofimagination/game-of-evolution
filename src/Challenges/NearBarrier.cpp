#include "NearBarrier.h"

#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
NearBarrier::NearBarrier(const Parameters& params)
    : m_Params(params)
{

}

//-------------------------------------------------------------------------
std::pair<bool, float> NearBarrier::PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    float radius;
    //radius = 20.0;
    radius = params.sizeX / 2;
    //radius = p.sizeX / 4;

    const std::vector<Coord> barrierCenters = grid.getBarrierCenters();
    float minDistance = 1e8;
    for (auto& center : barrierCenters) {
        float distance = (peep.loc - center).length();
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
    if (minDistance <= radius) {
        return { true, 1.0 - (minDistance / radius) };
    } else {
        return { false, 0.0 };
    }
};

} // namespace Challenges