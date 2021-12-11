#include "AlgorithmHelpers.h"

#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"

#include <cassert>
#include <cmath>

namespace AlgorithmHelpers
{

//-------------------------------------------------------------------------
void visitNeighborhood(Coord loc, float radius, std::function<void(Coord)> f, const Parameters& params)
{
    for (int dx = -std::min<int>(radius, loc.x); dx <= std::min<int>(radius, (params.sizeX - loc.x) - 1); ++dx) {
        int16_t x = loc.x + dx;
        assert(x >= 0 && x < params.sizeX);
        int extentY = (int)sqrt(radius * radius - dx * dx);
        for (int dy = -std::min<int>(extentY, loc.y); dy <= std::min<int>(extentY, (params.sizeY - loc.y) - 1); ++dy) {
            int16_t y = loc.y + dy;
            assert(y >= 0 && y < params.sizeY);
            f( Coord { x, y} );
        }
    }
}

//-------------------------------------------------------------------------
float getPopulationDensityAlongAxis(Coord loc, Dir dir, const Grid& grid, const Parameters& params)
{
    assert(dir != Compass::CENTER);  // require a defined axis

    double sum = 0.0;
    Coord dirVec = dir.asNormalizedCoord();
    double len = std::sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);
    double dirVecX = dirVec.x / len;
    double dirVecY = dirVec.y / len; // Unit vector components along dir

    auto f = [&](Coord tloc) {
        if (tloc != loc && grid.isOccupiedAt(tloc)) {
            Coord offset = tloc - loc;
            double proj = dirVecX * offset.x + dirVecY * offset.y; // Magnitude of projection along dir
            double contrib = proj / (offset.x * offset.x + offset.y * offset.y);
            sum += contrib;
        }
    };

    visitNeighborhood(loc, params.populationSensorRadius, f, params);

    double maxSumMag = 6.0 * params.populationSensorRadius;
    assert(sum >= -maxSumMag && sum <= maxSumMag);

    double sensorVal;
    sensorVal = sum / maxSumMag; // convert to -1.0..1.0
    sensorVal = (sensorVal + 1.0) / 2.0; // convert to 0.0..1.0

    return sensorVal;
}

//-------------------------------------------------------------------------
float getShortProbeBarrierDistance(Coord loc0, Dir dir, unsigned probeDistance, const Grid& grid)
{
    unsigned countFwd = 0;
    unsigned countRev = 0;
    Coord loc = loc0 + dir;
    unsigned numLocsToTest = probeDistance;
    // Scan positive direction
    while (numLocsToTest > 0 && grid.isInBounds(loc) && !grid.isBarrierAt(loc)) {
        ++countFwd;
        loc = loc + dir;
        --numLocsToTest;
    }
    if (numLocsToTest > 0 && !grid.isInBounds(loc)) {
        countFwd = probeDistance;
    }
    // Scan negative direction
    numLocsToTest = probeDistance;
    loc = loc0 - dir;
    while (numLocsToTest > 0 && grid.isInBounds(loc) && !grid.isBarrierAt(loc)) {
        ++countRev;
        loc = loc - dir;
        --numLocsToTest;
    }
    if (numLocsToTest > 0 && !grid.isInBounds(loc)) {
        countRev = probeDistance;
    }

    float sensorVal = ((countFwd - countRev) + probeDistance); // convert to 0..2*probeDistance
    sensorVal = (sensorVal / 2.0) / probeDistance; // convert to 0.0..1.0
    return sensorVal;
}

//-------------------------------------------------------------------------
unsigned LongProbePopulationFwd(const Peep& peep, const Grid& grid)
{
    assert(peep.longProbeDist > 0);
    unsigned count = 0;
    auto loc = peep.loc + peep.lastMoveDir;
    unsigned numLocsToTest = peep.longProbeDist;
    while (numLocsToTest > 0 && grid.isInBounds(loc) && grid.isEmptyAt(loc)) {
        ++count;
        loc = loc + peep.lastMoveDir;
        --numLocsToTest;
    }
    if (numLocsToTest > 0 && (!grid.isInBounds(loc) || grid.isBarrierAt(loc))) {
        return peep.longProbeDist;
    } else {
        return count;
    }
}

//-------------------------------------------------------------------------
unsigned LongProbeBarrierFwd(const Peep& peep, const Grid& grid)
{
    assert(peep.longProbeDist > 0);
    unsigned count = 0;
    auto loc = peep.loc + peep.lastMoveDir;
    unsigned numLocsToTest = peep.longProbeDist;
    while (numLocsToTest > 0 && grid.isInBounds(loc) && !grid.isBarrierAt(loc)) {
        ++count;
        loc = loc + peep.lastMoveDir;
        --numLocsToTest;
    }
    if (numLocsToTest > 0 && !grid.isInBounds(loc)) {
        return peep.longProbeDist;
    } else {
        return count;
    }
}

//-------------------------------------------------------------------------
float responseCurve(float r, const Parameters& params)
{
    const float k = params.responsivenessCurveKFactor;
    return std::pow((r - 2.0), -2.0 * k) - std::pow(2.0, -2.0 * k) * (1.0 - r);
}

//-------------------------------------------------------------------------
bool prob2bool(float factor, RandomUintGenerator& random)
{
    assert(factor >= 0.0 && factor <= 1.0);
    return (random() / (float)RANDOM_UINT_MAX) < factor;
}

} // namespace AlgorithmHelpers