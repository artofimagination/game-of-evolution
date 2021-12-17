#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are all those on the right side of the arena
class RightHalf : public iChallenge
{
public:
    struct Setup 
    {
        Coord topLeft;
        float width;
        float height;
    };

    RightHalf(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges