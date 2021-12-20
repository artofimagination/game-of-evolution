#include "iBarriers.h"
#include "Barriers/CircleBarrier.h"
#include "Barriers/RectangleBarrier.h"
#include "Parameters.h"

#include <cassert>

namespace Barriers
{

//-------------------------------------------------------------------------
void createBarrier(
    eBarrierType barrierType,
    std::vector<std::unique_ptr<iBarrier> >& barriers,
    RandomUintGenerator& random,
    const Parameters& params)
{
    barriers.clear();
    switch(barrierType) 
    {
    case eBarrierType::NoBarrier:
        return;

    // Vertical bar in constant location
    case eBarrierType::VerticalBarConstantLoc:
    {
        barriers.emplace_back(std::make_unique<Rectangle>(Coord(params.sizeX /2, params.sizeY / 4), 1, params.sizeY / 2));
        return;
    }
    // Vertical bar in random location
    case eBarrierType::VerticalBarRandomLoc:
    {
        auto topLeft = Coord(random(20, params.sizeX - 20), random(20, params.sizeY / 2 - 20));
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, 1, params.sizeY / 2));
        return;
    }
    // five blocks staggered
    case eBarrierType::FiveBlocksStaggered:
    {
        int16_t blockSizeX = 2;
        int16_t blockSizeY = params.sizeX / 3;
        auto topLeft = Coord(params.sizeX / 4 - blockSizeX / 2, params.sizeY / 4 - blockSizeY / 2);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, blockSizeX, blockSizeY));
        topLeft = Coord(topLeft.x + params.sizeX / 2, params.sizeY / 4 - blockSizeY / 2);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, blockSizeX, blockSizeY));
        topLeft = Coord(topLeft.x, topLeft.y + params.sizeY / 2);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, blockSizeX, blockSizeY));
        topLeft = Coord(topLeft.x - params.sizeX / 2, topLeft.y);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, blockSizeX, blockSizeY));
        topLeft = Coord(params.sizeX / 2 - blockSizeX / 2, params.sizeY / 2 - blockSizeY / 2);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, blockSizeX, blockSizeY));
        return;
    }
    // Horizontal bar in constant location
    case eBarrierType::HorizontalBarConstantLoc:
    {
        auto topLeft = Coord(params.sizeX / 4, params.sizeY / 2);
        barriers.emplace_back(std::make_unique<Rectangle>(topLeft, params.sizeX / 2, 2));
        return;
    }
    // Three floating islands -- different locations every generation
    case eBarrierType::ThreeFloatingIslands:
    {
        float radius = 3.0;
        unsigned margin = 2 * (int)radius;
        auto center0 = Coord((int16_t)random(margin, params.sizeX - margin),
                          (int16_t)random(margin, params.sizeY - margin) );
        barriers.emplace_back(std::make_unique<Circle>(center0, radius));

        Coord center1;
        Coord center2;
        do {
            center1 = Coord((int16_t)random(margin, params.sizeX - margin),
                          (int16_t)random(margin, params.sizeY - margin) );
        } while ((center0 - center1).length() < margin);

        do {
            center2 = Coord((int16_t)random(margin, params.sizeX - margin),
                          (int16_t)random(margin, params.sizeY - margin) );
        } while ((center0 - center2).length() < margin || (center1 - center2).length() < margin);
        barriers.emplace_back(std::make_unique<Circle>(center1, radius));
        barriers.emplace_back(std::make_unique<Circle>(center2, radius));
        return;
    }
    // Spots, specified number, radius, locations
    case eBarrierType::SpotsSpecified:
        {
            unsigned numberOfLocations = 5;
            float radius = 5.0;
            unsigned verticalSliceSize = params.sizeY / (numberOfLocations + 1);

            for (unsigned n = 1; n <= numberOfLocations; ++n) {
                Coord loc = { (int16_t)(params.sizeX / 2),
                              (int16_t)(n * verticalSliceSize) };
                barriers.emplace_back(std::make_unique<Circle>(loc, radius));
            }
            return;
        }
    default:
        assert(false);
    }
}

} // namespace Barriers