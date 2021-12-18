#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those touching any wall at the end of the generation
class AgainstAnyWall : public iChallenge
{
public:
    struct Setup 
    {
        std::vector<unsigned> borders; //!< 0 - left, 1 - top, 2 - right, 3 - bottom
    };

    AgainstAnyWall(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges