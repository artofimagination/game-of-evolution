#pragma once

#include "BasicTypes.h"
#include "iChallenges.h"

class RandomUintGenerator;

namespace Challenges
{

class Altruism : public iChallenge
{
public:
    struct Setup 
    {
        Coord center;
        float radius;
        Coord sacrificeCenter;
        float sacrificeRadius;
    };

    Altruism(RandomUintGenerator& random, const Parameters& params);

    std::vector<std::pair<uint16_t, float> >& EvaluateWhenNewGeneration(
        const PeepsPool& peeps,
        const Parameters& params,
        const Grid& grid,
        const Settings& settings) override; 

    const Setup& GetSetup() const { return m_Setup; }
 
private:
    std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) override;

    RandomUintGenerator& m_Random;
    const Parameters& m_Params;
    Setup m_Setup{}; 
};

} // namespace Challenges