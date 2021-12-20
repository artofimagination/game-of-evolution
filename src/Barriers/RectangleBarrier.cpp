#include "RectangleBarrier.h"

#include "AlgorithmHelpers.h"
#include "Grid.h"
#include "Parameters.h"

namespace Barriers
{

//-------------------------------------------------------------------------
Rectangle::Rectangle(const Coord& topLeft, uint16_t width, uint16_t height)
{
    m_Setup.topLeft = topLeft;
    m_Setup.width = width;
    m_Setup.height = height;
}

//-------------------------------------------------------------------------
void Rectangle::Draw(uint16_t barrierMask, const Parameters&, Grid& grid)
{
    for (int16_t x = m_Setup.topLeft.x; x <= m_Setup.topLeft.x + m_Setup.width; ++x) {
        for (int16_t y = m_Setup.topLeft.y; y <= m_Setup.topLeft.y + m_Setup.height; ++y) {
            grid.set(x, y, barrierMask);
        }
    }
}

} // namespace Barriers