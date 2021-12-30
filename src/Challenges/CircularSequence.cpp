#include "CircularSequence.h"

#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

#include <cassert>
#include <iostream>

namespace Challenges
{

//-------------------------------------------------------------------------
CircularSequence::CircularSequence(const Parameters& params)
{
    auto worldCenter = Coord {(int16_t)(params.sizeX / 2), (int16_t)( params.sizeY / 2)};
    auto distance = 2 * params.sizeX / 6;
    auto distanceCos = cos(0.785398163) * distance;  // Distance at 45 degs.
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x - distance), (int16_t)(worldCenter.y) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x - distanceCos), (int16_t)(worldCenter.y + distanceCos) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x), (int16_t)(worldCenter.y + distance) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x + distanceCos), (int16_t)(worldCenter.y + distanceCos) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x + distance), (int16_t)(worldCenter.y) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x + distanceCos), (int16_t)(worldCenter.y - distanceCos) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x), (int16_t)(worldCenter.y - distance) });
    m_Setup.centers.emplace_back(Coord{ (int16_t)(worldCenter.x - distanceCos), (int16_t)(worldCenter.y - distanceCos) });
    m_Setup.radius = params.sizeX / 10.0;

}

//-------------------------------------------------------------------------
std::vector<std::pair<uint16_t, float> >& CircularSequence::EvaluateWhenNewGeneration(
    const PeepsPool& peeps,
    const Parameters& params,
    const Grid& grid,
    const Settings& settings)
{
    for(auto countVal : m_PeepsTouchedCircles)
        std::cout << countVal << " ";
    std::cout << std::endl;
    m_PeepsTouchedCircles = {0, 0, 0, 0, 0, 0, 0, 0};
    return iChallenge::EvaluateWhenNewGeneration(peeps, params, grid, settings);
} 

//-------------------------------------------------------------------------
std::pair<bool, float> CircularSequence::PassedCriteria(const Peep& peep, const Parameters&, const Grid&) 
{ 
    unsigned count = 0;
    unsigned bits = peep.challengeBits;
    unsigned maxNumberOfBits = 8;

    for (unsigned n = 0; n < maxNumberOfBits; ++n) {
        if ((bits & (1 << n)) == 0) {
            break;
        }
        m_PeepsTouchedCircles.at(n)++;
        count++;
    }

    if (count > 0) {
        return { true, count / (float)maxNumberOfBits };
    } else {
        return { false, 0.05 };
    }
};

//-------------------------------------------------------------------------
void CircularSequence::EvaluateAtEndOfSimStep(
    PeepsPool& peeps,
    const Parameters& params,
    const Grid&,
    const Settings&)
{
    for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
        Peep &peep = peeps[index];
        for (unsigned n = 0; n < m_Setup.centers.size(); ++n) {
            unsigned bit = 1 << n;
            Coord offset = m_Setup.centers[n] - peep.loc;
            float distance = offset.length();
            if ((peep.challengeBits & bit) == 0) {
                if (distance <= m_Setup.radius) {
                    peep.challengeBits |= bit;
                }
                break;
            }
        }
    }
}

} // namespace Challenges