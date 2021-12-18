#pragma once

#include <memory>
#include <vector>

class Grid;
class Parameters;
class Peep;
class PeepsPool;
class RandomUintGenerator;

// Some of the survival challenges to try. Some are interesting, some
// not so much. Fine-tune the challenges by tweaking the corresponding code
// in survival-criteria.cpp.
enum class eChallenges : uint16_t
{
    Circle,
    RightHalf,
    RightQuarter,
    NeighborCount,
    CenterWeighted,
    CenterUnweighted,
    Corner,
    CornerWeighted,
    MigrateDistance,
    CenterSparsed,
    LeftEighth,
    RadioActiveWalls,
    AgainstAnyWall,
    TouchAnyWall,
    EastWestEighths,
    NearBarrier,
    Pairs,
    LocationSequence,
    Altruism,
    AltruismSacrifice,
    NoOfChallenges
};

namespace Challenges
{

struct Settings 
{
    unsigned generation{};  //!< Current generation number
    unsigned murderCount{};
    unsigned simStep{};     //!< Current simulation step.
};

class iChallenge
{
public:
    //! Evaluates challenge on all the peeps at the beginning of a new generation.
    //! \return the list of parents survived the challenge.
    virtual std::vector<std::pair<uint16_t, float> >& EvaluateWhenNewGeneration(
        const PeepsPool& peeps,
        const Parameters& params,
        const Grid& grid,
        const Settings& settings);

    //! Evaluates challenge on all the peeps at the end of simulation step.
    //! \return the list of parents survived the challenge.
    virtual void EvaluateAtEndOfSimStep(
        PeepsPool&,
        const Parameters&,
        const Grid&,
        const Settings&){};

protected:
    //! Checks whether the peep passes the challenge criteria.
    //! \return true and a score 0.0..1.0 if passed, false if failed.
    virtual std::pair<bool, float> PassedCriteria(const Peep& peep, const Parameters& params, const Grid& grid) = 0;

    //! Returns the surviveing parents.
    std::vector<std::pair<uint16_t, float> >& GetParents() { return m_Parents; }
private:
    //! This container will hold the indexes and survival scores (0.0..1.0)
    //! of all the survivors who will provide genomes for repopulation.
    std::vector<std::pair<uint16_t, float> > m_Parents; // <peep index, score>
};

//! Evaluates the seletced challenge on all the peeps.
//! \return the list of parents survived the challenge.
iChallenge* CreateChallenge(
    eChallenges challenge,
    RandomUintGenerator& random,
    const Parameters& params);

} // namespace Challenges