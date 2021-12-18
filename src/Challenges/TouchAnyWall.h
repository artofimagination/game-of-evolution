#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Peeps that are touching a wall are flagged in their Peep record. They are
//! allowed to continue living. Here at the end of the generation, any that
//! never touch a wall will die. All that touched a wall at any time during
//! their life will become parents.
class TouchAnyWall : public iChallenge
{
public:
    struct Setup 
    {
        std::vector<unsigned> borders; //!< 0 - left, 1 - top, 2 - right, 3 - bottom
    };

    TouchAnyWall(const Parameters& params);

    //! \copydoc iChallenge::EvaluateAtEndOfSimStep
    void EvaluateAtEndOfSimStep(
        PeepsPool& peeps,
        const Parameters&,
        const Grid&,
        const Settings& settings) override;

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges