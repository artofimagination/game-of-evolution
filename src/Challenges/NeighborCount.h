#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those not touching the border and with exactly the number
//! of neighbors defined by neighbors and radius, where neighbors includes self.
class NeighborCount : public iChallenge
{
public:
    struct Setup 
    {
        std::vector<unsigned> borders; //!< 0 - left, 1 - top, 2 - right, 3 - bottom
        unsigned minNeighbors;
        unsigned maxNeighbors;
        float radius;
    };

    NeighborCount(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges