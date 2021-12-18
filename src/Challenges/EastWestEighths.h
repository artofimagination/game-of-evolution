#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are all those on the left or right eighths of the arena
class EastWestEighths : public iChallenge
{
public:
    struct Setup 
    {
        Coord eastTopLeft;
        Coord westTopLeft;
        float width;
        float height;
    };

    EastWestEighths(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges