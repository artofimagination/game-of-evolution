#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! The more circle a peep gets inside in the predetermined order,
//! the higher its chances for survival are. Getting into all circles
//! is 100% survival. The order is starting from the left most circle
//! in counter clockwise direction.
//! If they don't get in a circle they survival chance is 5%.
class CircularSequence : public iChallenge
{
public:
    struct Setup 
    {
        std::vector<Coord> centers;
        float radius;
    };

    CircularSequence(const Parameters& params);

    //! \copydoc iChallenge::EvaluateAtEndOfSimStep
    void EvaluateAtEndOfSimStep(
        PeepsPool& peeps,
        const Parameters&,
        const Grid&,
        const Settings& settings) override;

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;


    std::vector<std::pair<uint16_t, float> >& EvaluateWhenNewGeneration(
        const PeepsPool& peeps,
        const Parameters& params,
        const Grid& grid,
        const Settings& settings) override; 

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    std::vector<unsigned> m_PeepsTouchedCircles{0, 0, 0, 0, 0, 0, 0, 0};
};

} // namespace Challenges