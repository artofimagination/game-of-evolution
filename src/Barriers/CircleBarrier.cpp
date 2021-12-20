#include "CircleBarrier.h"

#include "Grid.h"
#include "AlgorithmHelpers.h"

namespace Barriers
{

//-------------------------------------------------------------------------
Circle::Circle(const Coord& center, float radius)
{
    m_Setup.center = center;
    m_Setup.radius = radius;
}

//-------------------------------------------------------------------------
void Circle::Draw(uint16_t barrierMask, const Parameters& params, Grid& grid)
{
    auto f = [&](Coord loc) {
        grid.set(loc, barrierMask);
    };

    AlgorithmHelpers::visitNeighborhood(m_Setup.center, m_Setup.radius, f, params);
}

} // namespace Barriers