#include "iChallenges.h"

#include "Challenges/AgainstAnyWall.h"
#include "Challenges/Altruism.h"
#include "Challenges/AltruismSacrifice.h"
#include "Challenges/CenterSparsed.h"
#include "Challenges/CenterUnweighted.h"
#include "Challenges/CenterWeighted.h"
#include "Challenges/Circle.h"
#include "Challenges/CircularSequence.h"
#include "Challenges/Corner.h"
#include "Challenges/CornerWeighted.h"
#include "Challenges/EastWestEighths.h"
#include "Challenges/iChallenges.h"
#include "Challenges/LeftEighth.h"
#include "Challenges/LocationSequence.h"
#include "Challenges/MigrateDistance.h"
#include "Challenges/NearBarrier.h"
#include "Challenges/NeighborCount.h"
#include "Challenges/Pairs.h"
#include "Challenges/RightHalf.h"
#include "Challenges/RightQuarter.h"
#include "Challenges/RadioactiveWalls.h"
#include "Challenges/TouchAnyWall.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"

#include <cassert>

namespace Challenges
{

//-------------------------------------------------------------------------
std::vector<std::pair<uint16_t, float> >& iChallenge::EvaluateWhenNewGeneration(        
        const PeepsPool& peeps,
        const Parameters& params,
        const Grid& grid,
        const Settings&)
{
    m_Parents.clear();
    // First, make a list of all the peeps who will become parents; save
    // their scores for later sorting. Indexes start at 1.
    for (uint16_t index = 1; index <= params.population; ++index) {
        std::pair<bool, float> passed = PassedCriteria(peeps[index], params, grid);
        // Save the parent genome if it results in valid neural connections
        // ToDo: if the parents no longer need their genome record, we could
        // possibly do a move here instead of copy, although it's doubtful that
        // the optimization would be noticeable.
        if (passed.first && !peeps[index].nnet.connections.empty()) {
            m_Parents.push_back( { index, passed.second } );
        }
    }
    return m_Parents;
}

//-------------------------------------------------------------------------
iChallenge* CreateChallenge(
    eChallenges challenge,
    RandomUintGenerator& random,
    const Parameters& params)
{
    switch(challenge) {
        // Survivors are those inside the circular area defined by
        // safeCenter and radius
        case eChallenges::Circle:
            return new Circle(params);
        // Survivors are all those on the right side of the arena
        case eChallenges::RightHalf:
            return new RightHalf(params);
        // Survivors are all those on the right quarter of the arena
        case eChallenges::RightQuarter:
            return new RightQuarter(params);
        // Survivors are all those on the left eighth of the arena
        case eChallenges::LeftEighth:
            return new LeftEighth(params);
        // Survivors are those not touching the border and with exactly the number
        // of neighbors defined by neighbors and radius, where neighbors includes self
        case eChallenges::NeighborCount:
            return new NeighborCount(params);
        // Survivors are those within the specified radius of the center. The score
        // is linearly weighted by distance from the center.
        case eChallenges::CenterWeighted:
            return new CenterWeighted(params);
        // Survivors are those within the specified radius of the center
        case eChallenges::CenterUnweighted:
            return new CenterUnweighted(params);
        // Survivors are those within the specified outer radius of the center and with
        // the specified number of neighbors in the specified inner radius.
        // The score is not weighted by distance from the center.
        case eChallenges::CenterSparsed:
            return new CenterSparsed(params);
        // Survivors are those within the specified radius of any corner.
        // Assumes square arena.
        case eChallenges::Corner:
            return new Corner(params);
        // Survivors are those within the specified radius of any corner. The score
        // is linearly weighted by distance from the corner point.
        case eChallenges::CornerWeighted:
            return new CornerWeighted(params);
        // This challenge is handled in endOfSimStep(), where peeps may die
        // at the end of any sim step. There is nothing else to do here at the
        // end of a generation. All remaining alive become parents.
        case eChallenges::RadioActiveWalls:
            return new RadioactiveWalls(params, random);
        // Survivors are those touching any wall at the end of the generation
        case eChallenges::AgainstAnyWall:
            return new AgainstAnyWall(params);
        // This challenge is partially handled in endOfSimStep(), where peeps
        // that are touching a wall are flagged in their Peep record. They are
        // allowed to continue living. Here at the end of the generation, any that
        // never touch a wall will die. All that touched a wall at any time during
        // their life will become parents.
        case eChallenges::TouchAnyWall:
            return new TouchAnyWall(params);
        // Everybody survives and are candidate parents, but scored by how far
        // they migrated from their birth location.
        case eChallenges::MigrateDistance:
            return new MigrateDistance(params);
        // Survivors are all those on the left or right eighths of the arena
        case eChallenges::EastWestEighths:
            return new EastWestEighths(params);
        // Survivors are those within radius of any barrier center. Weighted by distance.
        case eChallenges::NearBarrier:
            return new NearBarrier(params);
        // Survivors are those not touching a border and with exactly one neighbor which has no other neighbor
        case eChallenges::Pairs:
            return new Pairs(params);
        // Survivors are those that contacted one or more specified locations in a sequence,
        // ranked by the number of locations contacted. There will be a bit set in their
        // challengeBits member for each location contacted.
        case eChallenges::LocationSequence:
            return new LocationSequence(params);
        // Survivors are all those within the specified radius of the NE corner
        case eChallenges::AltruismSacrifice:
            return new AltruismSacrifice(params);
        // Survivors are those inside the circular area defined by
        // safeCenter and radius
        case eChallenges::Altruism:
            return new Altruism(random, params);
        // Increasing survival chances with getting in more circles in a predetermined order.
        case eChallenges::CircularSequence:
            return new CircularSequence(params);
        default:
            assert(false);
            return new Altruism(random, params);
    }
    return new Altruism(random, params);
}

//-------------------------------------------------------------------------
std::vector<std::string> GetChallengeNames()
{
    std::vector<std::string> names{};
    for(eChallenges challenge = eChallenges::Circle; challenge < eChallenges::NoOfChallenges; challenge = static_cast<eChallenges>((size_t)challenge + 1))
    {
        switch(challenge) {
            case eChallenges::Circle:
                names.push_back("Circle");
                break;
            case eChallenges::RightHalf:
                names.push_back("RightHalf");
                break;
            case eChallenges::RightQuarter:
                names.push_back("RightQuarter");
                break;
            case eChallenges::LeftEighth:
                names.push_back("LeftEighth");
                break;
            case eChallenges::NeighborCount:
                names.push_back("NeighborCount");
                break;
            case eChallenges::CenterWeighted:
                names.push_back("CenterWeighted");
                break;
            case eChallenges::CenterUnweighted:
                names.push_back("CenterUnweighted");
                break;
            case eChallenges::CenterSparsed:
                names.push_back("CenterSparsed");
                break;
            case eChallenges::Corner:
                names.push_back("Corner");
                break;
            case eChallenges::CornerWeighted:
                names.push_back("CornerWeighted");
                break;
            case eChallenges::RadioActiveWalls:
                names.push_back("RadioActiveWalls");
                break;
            case eChallenges::AgainstAnyWall:
                names.push_back("AgainstAnyWall");
                break;
            case eChallenges::TouchAnyWall:
                names.push_back("TouchAnyWall");
                break;
            case eChallenges::MigrateDistance:
                names.push_back("MigrateDistance");
                break;
            case eChallenges::EastWestEighths:
                names.push_back("EastWestEighths");
                break;
            case eChallenges::NearBarrier:
                names.push_back("NearBarrier");
                break;
            case eChallenges::Pairs:
                names.push_back("Pairs");
                break;
            case eChallenges::LocationSequence:
                names.push_back("LocationSequence");
                break;
            case eChallenges::AltruismSacrifice:
                names.push_back("AltruismSacrifice");
                break;
            case eChallenges::Altruism:
                names.push_back("Altruism");
                break;
            case eChallenges::CircularSequence:
                names.push_back("CircularSequence");
                break;
            default:
                assert(false);
        }
    }
    return names;
}

} // namespace Challenges