#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those within radius of any barrier center. Weighted by distance.
class NearBarrier : public iChallenge
{
public:

    NearBarrier(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

private:
    const Parameters& m_Params;
};

} // namespace Challenges