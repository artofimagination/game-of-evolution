#include "iChallenges.h"

#include "Challenges/Altruism.h"
#include "Challenges/AltruismSacrifice.h"
#include "Challenges/CenterSparsed.h"
#include "Challenges/CenterUnweighted.h"
#include "Challenges/CenterWeighted.h"
#include "Challenges/Circle.h"
#include "Challenges/Corner.h"
#include "Challenges/CornerWeighted.h"
#include "Challenges/iChallenges.h"
#include "Challenges/LeftEighth.h"
#include "Challenges/RightHalf.h"
#include "Challenges/RightQuarter.h"
#include "Challenges/RadioactiveWalls.h"
#include "Challenges/NeighborCount.h"
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
        if (passed.first && peeps[index].nnet.connections.size() > 0) {
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
            break;

        // This challenge is partially handled in endOfSimStep(), where peeps
        // that are touching a wall are flagged in their Peep record. They are
        // allowed to continue living. Here at the end of the generation, any that
        // never touch a wall will die. All that touched a wall at any time during
        // their life will become parents.
        case eChallenges::TouchAnyWall:
            break;
        // Everybody survives and are candidate parents, but scored by how far
        // they migrated from their birth location.
        case eChallenges::MigrateDistance:
            break;

        // Survivors are all those on the left or right eighths of the arena
        case eChallenges::EastWestEights:
            break;

        // Survivors are those within radius of any barrier center. Weighted by distance.
        case eChallenges::NearBarrier:
            break;

        // Survivors are those not touching a border and with exactly one neighbor which has no other neighbor
        case eChallenges::Pairs:
            break;

        // Survivors are those that contacted one or more specified locations in a sequence,
        // ranked by the number of locations contacted. There will be a bit set in their
        // challengeBits member for each location contacted.
        case eChallenges::LocationSequence:
            break;

        // Survivors are all those within the specified radius of the NE corner
        case eChallenges::AltruismSacrifice:
            return new AltruismSacrifice(params);
        // Survivors are those inside the circular area defined by
        // safeCenter and radius
        case eChallenges::Altruism:
            return new Altruism(random, params);
        default:
            assert(false);
            return new Altruism(random, params);
    }
    return new Altruism(random, params);
}

} // namespace Challenges