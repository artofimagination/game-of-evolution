#include "LocationSequence.h"

#include "BasicTypes.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

namespace Challenges
{

//-------------------------------------------------------------------------
LocationSequence::LocationSequence(const Parameters& params)
    : m_Params(params)
{
}

//-------------------------------------------------------------------------
std::pair<bool, float> LocationSequence::PassedCriteria(const Peep& peep, const Parameters&, const Grid&) 
{ 
    unsigned count = 0;
    unsigned bits = peep.challengeBits;
    unsigned maxNumberOfBits = sizeof(bits) * 8;

    for (unsigned n = 0; n < maxNumberOfBits; ++n) {
        if ((bits & (1 << n)) != 0) {
            ++count;
        }
    }
    if (count > 0) {
        return { true, count / (float)maxNumberOfBits };
    } else {
        return { false, 0.0 };
    }
};

//-------------------------------------------------------------------------
void LocationSequence::EvaluateAtEndOfSimStep(
    PeepsPool& peeps,
    const Parameters& params,
    const Grid& grid,
    const Settings&)
{
    float radius = 15.0;
    for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
        Peep &peep = peeps[index];
        for (unsigned n = 0; n < grid.getBarrierCenters().size(); ++n) {
            unsigned bit = 1 << n;
            if ((peep.challengeBits & bit) == 0) {
                if ((peep.loc - grid.getBarrierCenters()[n]).length() <= radius) {
                    peep.challengeBits |= bit;
                }
                break;
            }
        }
    }
}

} // namespace Challenges