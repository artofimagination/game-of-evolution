#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

class AltruismSacrifice : public iChallenge
{
public:
    struct Setup 
    {
        Coord center;
        float radius;
    };

    AltruismSacrifice(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges