#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

// Survivors are those within the specified radius of any corner. The score
// is linearly weighted by distance from the corner point.
class CornerWeighted : public iChallenge
{
public:

    struct Setup 
    {
        std::vector<Coord> centers;
        float radius;
    };

    CornerWeighted(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges