#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

namespace Challenges
{

//! Survivors are those within the specified outer radius of the center and with
//! the specified number of neighbors in the specified inner radius.
//! The score is not weighted by distance from the center.
class CenterSparsed : public iChallenge
{
public:
    struct Setup 
    {
        Coord center;
        float radius;
        float neighborRadius;
        unsigned minNeighbors;
        unsigned maxNeighbors;
    };

    CenterSparsed(const Parameters& params);

    //! \copydoc iChallenge::PassedCriteria
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    const Setup& GetSetup() const { return m_Setup; }

private:
    Setup m_Setup{};
    const Parameters& m_Params;
};

} // namespace Challenges