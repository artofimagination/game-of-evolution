#include "Grid.h"

#include "AlgorithmHelpers.h"
#include "Parameters.h"

#include <cassert>

constexpr uint16_t EMPTY = 0; // Index value 0 is reserved
constexpr uint16_t BARRIER = 0xffff;

//-------------------------------------------------------------------------
Grid::Grid(const Parameters& params, RandomUintGenerator& r)
    : m_Params(params)
    , m_RandomGenerator(r)
{

}

//-------------------------------------------------------------------------
void Grid::init()
{
    auto col = Column(m_Params.sizeY);
    data = std::vector<Column>(m_Params.sizeX, col);
}

//-------------------------------------------------------------------------
bool Grid::isEmptyAt(Coord loc) const 
{ 
    return at(loc) == EMPTY; 
}

//-------------------------------------------------------------------------
bool Grid::isOccupiedAt(Coord loc) const 
{ 
    return at(loc) != EMPTY && at(loc) != BARRIER; 
}

//-------------------------------------------------------------------------
bool Grid::isBarrierAt(Coord loc) const 
{ 
    return at(loc) == BARRIER; 
}

//-------------------------------------------------------------------------
void Grid::createBarrier(unsigned barrierType)
{
    barrierLocations.clear();
    barrierCenters.clear();  // used only for some barrier types

    auto drawBox = [&](int16_t minX, int16_t minY, int16_t maxX, int16_t maxY) {
        for (int16_t x = minX; x <= maxX; ++x) {
            for (int16_t y = minY; y <= maxY; ++y) {
                set(x, y, BARRIER);
                barrierLocations.push_back( {x, y} );
            }
        }
    };

    switch(barrierType) {
    case 0:
        return;

    // Vertical bar in constant location
    case 1:
        {
            int16_t minX = m_Params.sizeX / 2;
            int16_t maxX = minX + 1;
            int16_t minY = m_Params.sizeY / 4;
            int16_t maxY = minY + m_Params.sizeY / 2;

            for (int16_t x = minX; x <= maxX; ++x) {
                for (int16_t y = minY; y <= maxY; ++y) {
                    set(x, y, BARRIER);
                    barrierLocations.push_back( {x, y} );
                }
            }
        }
        break;

    // Vertical bar in random location
    case 2:
        {
            int16_t minX = m_RandomGenerator(20, m_Params.sizeX - 20);
            int16_t maxX = minX + 1;
            int16_t minY = m_RandomGenerator(20, m_Params.sizeY / 2 - 20);
            int16_t maxY = minY + m_Params.sizeY / 2;

            for (int16_t x = minX; x <= maxX; ++x) {
                for (int16_t y = minY; y <= maxY; ++y) {
                    set(x, y, BARRIER);
                    barrierLocations.push_back( {x, y} );
                }
            }
        }
        break;

    // five blocks staggered
    case 3:
        {
            int16_t blockSizeX = 2;
            int16_t blockSizeY = m_Params.sizeX / 3;

            int16_t x0 = m_Params.sizeX / 4 - blockSizeX / 2;
            int16_t y0 = m_Params.sizeY / 4 - blockSizeY / 2;
            int16_t x1 = x0 + blockSizeX;
            int16_t y1 = y0 + blockSizeY;

            drawBox(x0, y0, x1, y1);
            x0 += m_Params.sizeX / 2;
            x1 = x0 + blockSizeX;
            drawBox(x0, y0, x1, y1);
            y0 += m_Params.sizeY / 2;
            y1 = y0 + blockSizeY;
            drawBox(x0, y0, x1, y1);
            x0 -= m_Params.sizeX / 2;
            x1 = x0 + blockSizeX;
            drawBox(x0, y0, x1, y1);
            x0 = m_Params.sizeX / 2 - blockSizeX / 2;
            x1 = x0 + blockSizeX;
            y0 = m_Params.sizeY / 2 - blockSizeY / 2;
            y1 = y0 + blockSizeY;
            drawBox(x0, y0, x1, y1);
            return;
        }
        break;

    // Horizontal bar in constant location
    case 4:
        {
            int16_t minX = m_Params.sizeX / 4;
            int16_t maxX = minX + m_Params.sizeX / 2;
            int16_t minY = m_Params.sizeY / 2 + m_Params.sizeY / 4;
            int16_t maxY = minY + 2;

            for (int16_t x = minX; x <= maxX; ++x) {
                for (int16_t y = minY; y <= maxY; ++y) {
                    set(x, y, BARRIER);
                    barrierLocations.push_back( {x, y} );
                }
            }
        }
        break;

    // Three floating islands -- different locations every generation
    case 5:
        {
            float radius = 3.0;
            unsigned margin = 2 * (int)radius;

            auto randomLoc = [&]() {
//                return Coord( (int16_t)randomUint((int)radius + margin, p.sizeX - ((float)radius + margin)),
//                              (int16_t)randomUint((int)radius + margin, p.sizeY - ((float)radius + margin)) );
                return Coord( (int16_t)m_RandomGenerator(margin, m_Params.sizeX - margin),
                              (int16_t)m_RandomGenerator(margin, m_Params.sizeY - margin) );
            };

            Coord center0 = randomLoc();
            Coord center1;
            Coord center2;

            do {
                center1 = randomLoc();
            } while ((center0 - center1).length() < margin);

            do {
                center2 = randomLoc();
            } while ((center0 - center2).length() < margin || (center1 - center2).length() < margin);

            barrierCenters.push_back(center0);
            //barrierCenters.push_back(center1);
            //barrierCenters.push_back(center2);

            auto f = [&](Coord loc) {
                set(loc, BARRIER);
                barrierLocations.push_back(loc);
            };

            AlgorithmHelpers::visitNeighborhood(center0, radius, f, m_Params);
            //visitNeighborhood(center1, radius, f);
            //visitNeighborhood(center2, radius, f);
        }
        break;

    // Spots, specified number, radius, locations
    case 6:
        {
            unsigned numberOfLocations = 5;
            float radius = 5.0;

            auto f = [&](Coord loc) {
                set(loc, BARRIER);
                barrierLocations.push_back(loc);
            };

            unsigned verticalSliceSize = m_Params.sizeY / (numberOfLocations + 1);

            for (unsigned n = 1; n <= numberOfLocations; ++n) {
                Coord loc = { (int16_t)(m_Params.sizeX / 2),
                              (int16_t)(n * verticalSliceSize) };
                AlgorithmHelpers::visitNeighborhood(loc, radius, f, m_Params);
                barrierCenters.push_back(loc);
            }
        }
        break;

    default:
        assert(false);
    }
}

//-------------------------------------------------------------------------
Coord Grid::findEmptyLocation() const {
    Coord loc;
    bool found = false;

    while (!found) {
        loc.x = m_RandomGenerator(0, m_Params.sizeX - 1);
        loc.y = m_RandomGenerator(0, m_Params.sizeY - 1);
        if (isEmptyAt(loc)) {
            break;
        }
    }
    return loc;
}