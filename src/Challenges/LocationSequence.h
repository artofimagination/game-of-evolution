#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those that contacted one or more specified locations in a sequence,
//! ranked by the number of locations contacted. There will be a bit set in their
//! challengeBits member for each location contacted.
class LocationSequence : public iChallenge
{
public:

    LocationSequence(const Parameters& params);
    
    //! \copydoc iChallenge::EvaluateAtEndOfSimStep
    void EvaluateAtEndOfSimStep(
        PeepsPool& peeps,
        const Parameters&,
        const Grid&,
        const Settings& settings) override;

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters&, const Grid&) override;

private:
    const Parameters& m_Params;
};

} // namespace Challenges