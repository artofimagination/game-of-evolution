#include "Pairs.h"

#include "BasicTypes.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

namespace Challenges
{

//-------------------------------------------------------------------------
Pairs::Pairs(const Parameters& params)
    : m_Params(params)
{
    m_Setup.borders = { 0, 1, 2, 3 };
}

//-------------------------------------------------------------------------
std::pair<bool, float> Pairs::PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid)
{
    if (!peep.alive)
        return { false, 0.0 };
    
    bool onEdge = peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
                || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1;

    if (onEdge) {
        return { false, 0.0 };
    }

    unsigned count = 0;
    for (int16_t x = peep.loc.x - 1; x < peep.loc.x + 1; ++x) {
        for (int16_t y = peep.loc.y - 1; y < peep.loc.y + 1; ++y) {
            Coord tloc = { x, y };
            if (tloc != peep.loc && grid.isInBounds(tloc) && grid.isOccupiedAt(tloc)) {
                ++count;
                if (count == 1) {
                    for (int16_t x1 = tloc.x - 1; x1 < tloc.x + 1; ++x1) {
                        for (int16_t y1 = tloc.y - 1; y1 < tloc.y + 1; ++y1) {
                            Coord tloc1 = { x1, y1 };
                            if (tloc1 != tloc && tloc1 != peep.loc && grid.isInBounds(tloc1) && grid.isOccupiedAt(tloc1)) {
                                return { false, 0.0 };
                            }
                        }
                    }
                } else {
                    return { false, 0.0 };
                }
            }
        }
    }
    if (count == 1) {
        return { true, 1.0 };
    } else {
        return { false, 0.0 };
    }
};

} // namespace Challenges