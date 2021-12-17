#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those within the specified radius of the center. The score
//! is linearly weighted by distance from the center.
class CenterWeighted : public iChallenge
{
public:
    struct Setup 
    {
        Coord safeCenter;
        float radius;
    };

    CenterWeighted(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges