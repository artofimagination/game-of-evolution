#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! During the first half of the generation, the west wall is radioactive,
//! where X == 0. In the last half of the generation, the east wall is
//! radioactive, where X = the area width - 1. There's an exponential
//! falloff of the danger, falling off to zero at the arena half line.
class RadioactiveWalls : public iChallenge
{
public:
    struct Setup 
    {
        unsigned border{}; //!< 0 - left, 1 - top, 2 - right, 3 - bottom
        unsigned distance{};
    };

    RadioactiveWalls(const Parameters& params, RandomUintGenerator& random);

    //! \copydoc iChallenge::EvaluateAtEndOfSimStep
    void EvaluateAtEndOfSimStep(
        PeepsPool& peeps,
        const Parameters&,
        const Grid&,
        const Settings& settings) override;

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep&, const Parameters&, const Grid&) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
    RandomUintGenerator& m_Random;
};

} // namespace Challenges