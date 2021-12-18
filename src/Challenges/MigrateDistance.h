#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Everybody survives and are candidate parents, but scored by how far
//! they migrated from their birth location.
class MigrateDistance : public iChallenge
{
public:

    MigrateDistance(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

private:
    const Parameters& m_Params;
};

} // namespace Challenges