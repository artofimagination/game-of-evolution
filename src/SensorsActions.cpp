#include "SensorsActions.h"

#include "AlgorithmHelpers.h"
#include "Grid.h"
#include "Parameters.h"
#include "Peep.h"
#include "PeepsPool.h"
#include "PheromoneSignals.h"
#include "Random.h"

#include <cassert>
#include <limits.h>
#include <iostream>

namespace SensorsActions
{

//-------------------------------------------------------------------------
std::string sensorName(Sensors::eType sensor)
{
    switch(sensor) {
    case Sensors::eType::AGE: return "age"; break;
    case Sensors::eType::BOUNDARY_DIST: return "boundary dist"; break;
    case Sensors::eType::BOUNDARY_DIST_X: return "boundary dist X"; break;
    case Sensors::eType::BOUNDARY_DIST_Y: return "boundary dist Y"; break;
    case Sensors::eType::LAST_MOVE_DIR_X: return "last move dir X"; break;
    case Sensors::eType::LAST_MOVE_DIR_Y: return "last move dir Y"; break;
    case Sensors::eType::LOC_X: return "loc X"; break;
    case Sensors::eType::LOC_Y: return "loc Y"; break;
    case Sensors::eType::LONGPROBE_POP_FWD: return "long probe population fwd"; break;
    case Sensors::eType::LONGPROBE_BAR_FWD: return "long probe barrier fwd"; break;
    case Sensors::eType::BARRIER_FWD: return "short probe barrier fwd-rev"; break;
    case Sensors::eType::BARRIER_LR: return "short probe barrier left-right"; break;
    case Sensors::eType::OSC1: return "osc1"; break;
    case Sensors::eType::POPULATION: return "population"; break;
    case Sensors::eType::POPULATION_FWD: return "population fwd"; break;
    case Sensors::eType::POPULATION_LR: return "population LR"; break;
    case Sensors::eType::RANDOM: return "random"; break;
    case Sensors::eType::SIGNAL0: return "signal 0"; break;
    case Sensors::eType::SIGNAL0_FWD: return "signal 0 fwd"; break;
    case Sensors::eType::SIGNAL0_LR: return "signal 0 LR"; break;
    case Sensors::eType::GENETIC_SIM_FWD: return "genetic similarity fwd"; break;
    case Sensors::eType::PlannedLocX: return "planned loc x diff"; break;
    case Sensors::eType::PlannedLocY: return "planned loc y diff"; break;
    case Sensors::eType::PlannedLocTime: return "planned time diff"; break;
    case Sensors::eType::ChallengeSuccess: return "challenge success"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
std::string sensorShortName(Sensors::eType sensor)
{
    switch(sensor) {
    case Sensors::eType::AGE: return "Age"; break;
    case Sensors::eType::BOUNDARY_DIST: return "ED"; break;
    case Sensors::eType::BOUNDARY_DIST_X: return "EDx"; break;
    case Sensors::eType::BOUNDARY_DIST_Y: return "EDy"; break;
    case Sensors::eType::LAST_MOVE_DIR_X: return "LMx"; break;
    case Sensors::eType::LAST_MOVE_DIR_Y: return "LMy"; break;
    case Sensors::eType::LOC_X: return "Lx"; break;
    case Sensors::eType::LOC_Y: return "Ly"; break;
    case Sensors::eType::LONGPROBE_POP_FWD: return "LPf"; break;
    case Sensors::eType::LONGPROBE_BAR_FWD: return "LPb"; break;
    case Sensors::eType::BARRIER_FWD: return "Bfd"; break;
    case Sensors::eType::BARRIER_LR: return "Blr"; break;
    case Sensors::eType::OSC1: return "Osc"; break;
    case Sensors::eType::POPULATION: return "Pop"; break;
    case Sensors::eType::POPULATION_FWD: return "Pfd"; break;
    case Sensors::eType::POPULATION_LR: return "Plr"; break;
    case Sensors::eType::RANDOM: return "Rnd"; break;
    case Sensors::eType::SIGNAL0: return "Sg"; break;
    case Sensors::eType::SIGNAL0_FWD: return "Sfd"; break;
    case Sensors::eType::SIGNAL0_LR: return "Slr"; break;
    case Sensors::eType::GENETIC_SIM_FWD: return "Gen"; break;
    case Sensors::eType::PlannedLocX: return "PXd"; break;
    case Sensors::eType::PlannedLocY: return "PYd"; break;
    case Sensors::eType::PlannedLocTime: return "Ptd"; break;
    case Sensors::eType::ChallengeSuccess: return "ChS"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
std::string actionShortName(Actions::eType action)
{
    switch(action) {
    case Actions::eType::MOVE_EAST: return "MvE"; break;
    case Actions::eType::MOVE_WEST: return "MvW"; break;
    case Actions::eType::MOVE_NORTH: return "MvN"; break;
    case Actions::eType::MOVE_SOUTH: return "MvS"; break;
    case Actions::eType::MOVE_NE: return "MNe"; break;
    case Actions::eType::MOVE_SE: return "MSe"; break;
    case Actions::eType::MOVE_SW: return "MSw"; break;
    case Actions::eType::MOVE_NW: return "MNw"; break;
    case Actions::eType::SET_RESPONSIVENESS: return "Res"; break;
    case Actions::eType::SET_OSCILLATOR_PERIOD: return "OSC"; break;
    case Actions::eType::EMIT_SIGNAL0: return "SG"; break;
    case Actions::eType::KILL_FORWARD: return "Klf"; break;
    case Actions::eType::MOVE_RANDOM: return "Mrn"; break;
    case Actions::eType::SET_LONGPROBE_DIST: return "LPD"; break;
    case Actions::eType::PlanPosX: return "PlX"; break;
    case Actions::eType::PlanPosY: return "PlY"; break;
    case Actions::eType::PlanTime: return "Pss"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
std::string actionName(Actions::eType action)
{
    switch(action) {
    case Actions::eType::MOVE_EAST: return "move east"; break;
    case Actions::eType::MOVE_WEST: return "move west"; break;
    case Actions::eType::MOVE_NORTH: return "move north"; break;
    case Actions::eType::MOVE_SOUTH: return "move south"; break;
    case Actions::eType::MOVE_NE: return "move north east"; break;
    case Actions::eType::MOVE_SE: return "move south east"; break;
    case Actions::eType::MOVE_SW: return "move south west"; break;
    case Actions::eType::SET_RESPONSIVENESS: return "set inv-responsiveness"; break;
    case Actions::eType::SET_OSCILLATOR_PERIOD: return "set osc1"; break;
    case Actions::eType::EMIT_SIGNAL0: return "emit signal 0"; break;
    case Actions::eType::KILL_FORWARD: return "kill fwd"; break;
    case Actions::eType::MOVE_NW: return "move north west"; break;
    case Actions::eType::MOVE_RANDOM: return "move random"; break;
    case Actions::eType::SET_LONGPROBE_DIST: return "set longprobe dist"; break;
    case Actions::eType::PlanPosX: return "planned loc x"; break;
    case Actions::eType::PlanPosY: return "planned loc y"; break;
    case Actions::eType::PlanTime: return "planned sim step"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
float getSignalDensity(unsigned layerNum, Coord loc, const PheromoneSignals& pheromoneSignals, const Parameters& params)
{
    unsigned countLocs = 0;
    unsigned long sum = 0;
    Coord center = loc;

    auto f = [&](Coord tloc) {
        ++countLocs;
        sum += pheromoneSignals.getMagnitude(layerNum, tloc);
    };

    AlgorithmHelpers::visitNeighborhood(center, params.signalSensorRadius, f, params);
    double maxSum = (float)countLocs * SIGNAL_MAX;
    double sensorVal = sum / maxSum; // convert to 0.0..1.0

    return sensorVal;
}

//-------------------------------------------------------------------------
float getSignalDensityAlongAxis(unsigned layerNum, Coord loc, Dir dir,  const PheromoneSignals& pheromoneSignals, const Parameters& params)
{
    assert(dir != Compass::CENTER); // require a defined axis

    double sum = 0.0;
    Coord dirVec = dir.asNormalizedCoord();
    double len = std::sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);
    double dirVecX = dirVec.x / len;
    double dirVecY = dirVec.y / len; // Unit vector components along dir

    auto f = [&](Coord tloc) {
        if (tloc != loc) {
            Coord offset = tloc - loc;
            double proj = (dirVecX * offset.x + dirVecY * offset.y); // Magnitude of projection along dir
            double contrib = (proj * pheromoneSignals.getMagnitude(layerNum, loc)) /
                    (offset.x * offset.x + offset.y * offset.y);
            sum += contrib;
        }
    };

    AlgorithmHelpers::visitNeighborhood(loc, params.signalSensorRadius, f, params);

    double maxSumMag = 6.0 * params.signalSensorRadius * SIGNAL_MAX;
    assert(sum >= -maxSumMag && sum <= maxSumMag);
    double sensorVal = sum / maxSumMag; // convert to -1.0..1.0
    sensorVal = (sensorVal + 1.0) / 2.0; // convert to 0.0..1.0

    return sensorVal;
}

//-------------------------------------------------------------------------
void displaySensorActionReferenceCounts(const PeepsPool& peeps, const Parameters& params)
{
    std::vector<unsigned> sensorCounts(Sensors::eType::NUM_SENSES, 0);
    std::vector<unsigned> actionCounts(Actions::eType::NUM_ACTIONS, 0);

    for (unsigned index = 1; index <= params.population; ++index) {
        if (peeps[index].alive) {
            const Peep &peep = peeps[index];
            for (const auto& gene : peep.nnet.connections) {
                if (gene.sourceType == Genetics::SENSOR) {
                    assert(gene.sourceNum < Sensors::eType::NUM_SENSES);
                    ++sensorCounts[(Sensors::eType)gene.sourceNum];
                }
                if (gene.sinkType == Genetics::ACTION) {
                    assert(gene.sinkNum < Actions::eType::NUM_ACTIONS);
                    ++actionCounts[(Actions::eType)gene.sinkNum];
                }
            }
        }
    }

    // std::cout << "Sensors in use:" << std::endl;
    // for (unsigned i = 0; i < sensorCounts.size(); ++i) {
    //     if (sensorCounts[i] > 0) {
    //         std::cout << "  " << sensorCounts[i] << " - " << sensorName((Sensors::eType)i) << std::endl;
    //     }
    // }
    // std::cout << "Actions in use:" << std::endl;
    // for (unsigned i = 0; i < actionCounts.size(); ++i) {
    //     if (actionCounts[i] > 0) {
    //         std::cout << "  " << actionCounts[i] << " - " << actionName((Actions::eType)i) << std::endl;
    //     }
    // }
}

} // namespace SensorsActions

//-------------------------------------------------------------------------
float Sensors::getSensor(
    const Peep& peep,
    const PeepsPool& peeps,
    uint8_t sensorTypeIndex,
    unsigned simStep,
    unsigned oldestAge,
    const Grid& grid,
    const Parameters& params,
    RandomUintGenerator& random,
    const PheromoneSignals& pheromoneSignals) const
{
    assert(sensorTypeIndex < eType::NUM_SENSES);
    auto sensorType = m_AvailableTypes.at(sensorTypeIndex);
    float sensorVal = 0.0;

    switch (sensorType) {
    case eType::AGE:
        // Converts age (units of simSteps compared to life expectancy)
        // linearly to normalized sensor range 0.0..1.0
        sensorVal = (float)peep.age / oldestAge == 0 ? params.stepsPerGeneration : oldestAge;
        break;
    case eType::BOUNDARY_DIST:
    {
        // Finds closest boundary, compares that to the max possible dist
        // to a boundary from the center, and converts that linearly to the
        // sensor range 0.0..1.0
        int distX = std::min<int>(peep.loc.x, (params.sizeX - peep.loc.x) - 1);
        int distY = std::min<int>(peep.loc.y, (params.sizeY - peep.loc.y) - 1);
        int closest = std::min<int>(distX, distY);
        int maxPossible = std::max<int>(params.sizeX / 2 - 1, params.sizeY / 2 - 1);
        sensorVal = (float)closest / maxPossible;
        break;
    }
    case eType::BOUNDARY_DIST_X:
    {
        // Measures the distance to nearest boundary in the east-west axis,
        // max distance is half the grid width; scaled to sensor range 0.0..1.0.
        int minDistX = std::min<int>(peep.loc.x, (params.sizeX - peep.loc.x) - 1);
        sensorVal = minDistX / (params.sizeX / 2.0);
        break;
    }
    case eType::BOUNDARY_DIST_Y:
    {
        // Measures the distance to nearest boundary in the south-north axis,
        // max distance is half the grid height; scaled to sensor range 0.0..1.0.
        int minDistY = std::min<int>(peep.loc.y, (params.sizeY - peep.loc.y) - 1);
        sensorVal = minDistY / (params.sizeY / 2.0);
        break;
    }
    case eType::LAST_MOVE_DIR_X:
    {
        // X component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastX = peep.lastMoveDir.asNormalizedCoord().x;
        sensorVal = lastX == 0 ? 0.5 :
                (lastX == -1 ? 0.0 : 1.0);
        break;
    }
    case eType::LAST_MOVE_DIR_Y:
    {
        // Y component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastY = peep.lastMoveDir.asNormalizedCoord().y;
        sensorVal = lastY == 0 ? 0.5 :
                (lastY == -1 ? 0.0 : 1.0);
        break;
    }
    case eType::LOC_X:
        // Maps current X location 0..p.sizeX-1 to sensor range 0.0..1.0
        sensorVal = (float)peep.loc.x / (params.sizeX - 1);
        break;
    case eType::LOC_Y:
        // Maps current Y location 0..p.sizeY-1 to sensor range 0.0..1.0
        sensorVal = (float)peep.loc.y / (params.sizeY - 1);
        break;
    case eType::OSC1:
    {
        // Maps the oscillator sine wave to sensor range 0.0..1.0;
        // cycles starts at simStep 0 for everbody.
        float phase = (simStep % peep.oscPeriod) / (float)peep.oscPeriod; // 0.0..1.0
        float factor = -std::cos(phase * 2.0f * 3.1415927f);
        assert(factor >= -1.0f && factor <= 1.0f);
        factor += 1.0f;    // convert to 0.0..2.0
        factor /= 2.0;     // convert to 0.0..1.0
        sensorVal = factor;
        // Clip any round-off error
        sensorVal = std::min<float>(1.0, std::max<float>(0.0, sensorVal));
        break;
    }
    case eType::LONGPROBE_POP_FWD:
    {
        // Measures the distance to the nearest other individual in the
        // forward direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = AlgorithmHelpers::LongProbePopulationFwd(peep, grid) / (float)peep.longProbeDist; // 0..1
        break;
    }
    case eType::LONGPROBE_BAR_FWD:
    {
        // Measures the distance to the nearest barrier in the forward
        // direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = AlgorithmHelpers::LongProbeBarrierFwd(peep, grid) / (float)peep.longProbeDist; // 0..1
        break;
    }
    case eType::POPULATION:
    {
        // Returns population density in neighborhood converted linearly from
        // 0..100% to sensor range
        unsigned countLocs = 0;
        unsigned countOccupied = 0;
        Coord center = peep.loc;

        auto f = [&](Coord tloc) {
            ++countLocs;
            if (grid.isOccupiedAt(tloc)) {
                ++countOccupied;
            }
        };

        AlgorithmHelpers::visitNeighborhood(center, params.populationSensorRadius, f, params);
        sensorVal = (float)countOccupied / countLocs;
        break;
    }
    case eType::POPULATION_FWD:
        // Sense population density along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getPopulationDensityAlongAxis(peep.loc, peep.lastMoveDir, grid, params);
        break;
    case eType::POPULATION_LR:
        // Sense population density along an axis 90 degrees from last movement direction
        sensorVal = AlgorithmHelpers::getPopulationDensityAlongAxis(peep.loc, peep.lastMoveDir.rotate90DegCW(), grid, params);
        break;
    case eType::BARRIER_FWD:
        // Sense the nearest barrier along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getShortProbeBarrierDistance(peep.loc, peep.lastMoveDir, params.shortProbeBarrierDistance, grid);
        break;
    case eType::BARRIER_LR:
        // Sense the nearest barrier along axis perpendicular to last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getShortProbeBarrierDistance(peep.loc, peep.lastMoveDir.rotate90DegCW(), params.shortProbeBarrierDistance, grid);
        break;
    case eType::RANDOM:
        // Returns a random sensor value in the range 0.0..1.0.
        sensorVal = random() / (float)UINT_MAX;
        break;
    case eType::SIGNAL0:
        // Returns magnitude of signal0 in the local neighborhood, with
        // 0.0..maxSignalSum converted to sensorRange 0.0..1.0
        sensorVal = SensorsActions::getSignalDensity(0, peep.loc, pheromoneSignals, params);
        break;
    case eType::SIGNAL0_FWD:
        // Sense signal0 density along axis of last movement direction
        sensorVal = SensorsActions::getSignalDensityAlongAxis(0, peep.loc, peep.lastMoveDir, pheromoneSignals, params);
        break;
    case eType::SIGNAL0_LR:
        // Sense signal0 density along an axis perpendicular to last movement direction
        sensorVal = SensorsActions::getSignalDensityAlongAxis(0, peep.loc, peep.lastMoveDir.rotate90DegCW(), pheromoneSignals, params);
        break;
    case eType::GENETIC_SIM_FWD:
    {
        // Return minimum sensor value if nobody is alive in the forward adjacent location,
        // else returns a similarity match in the sensor range 0.0..1.0
        Coord loc2 = peep.loc + peep.lastMoveDir;
        if (grid.isInBounds(loc2) && grid.isOccupiedAt(loc2)) {
            const Peep &peep2 = peeps.getPeep(loc2);
            if (peep2.alive) {
                sensorVal = Genetics::genomeSimilarity(peep.genome, peep2.genome, params); // 0.0..1.0
            }
        }
        break;
    }
    case eType::PlannedLocX:
        // Gets the difference between the planned and actual loc x converts it to 0..1.0 range
        // where 0 means +-sizeX difference 1.0 means 0 difference.
        sensorVal = 1.0 - (abs(peep.plannedLoc.x - peep.loc.x) / float(params.sizeX));
        break;
    case eType::PlannedLocY:
        // Gets the difference between the planned and actual loc y converts it to 0..1.0 range
        // where 0 means +-sizeY difference 1.0 means 0 difference.
        sensorVal = 1.0 - (abs(peep.plannedLoc.y - peep.loc.y) / float(params.sizeY));
        break;
    case eType::PlannedLocTime:
        // Gets the difference between the planned and actual sim step time converts it to 0..1.0 range
        // where 0 means the max difference possible 1.0 the planned time is reached.
        assert(params.stepsPerGeneration > 0);
        sensorVal = 1.0 - (abs(peep.plannedSimStep - simStep) / float(params.stepsPerGeneration - peep.planTimeUpdateStep));
        break;
    case eType::ChallengeSuccess:
    {
        // Gets the number of completed challenge tasks and normalizes to 0..1.0.
        // It simulates success feeling.
        auto maxNumberOfBits = 8;
        sensorVal = peep.challengeBits / static_cast<float>(maxNumberOfBits);
        break;
    }
    default:
        assert(false);
        break;
    }

    if (std::isnan(sensorVal) || sensorVal < -0.01 || sensorVal > 1.01) {
        //std::cout << "sensorVal=" << (int)sensorVal << " for " << SensorsActions::sensorName((eType)sensorType) << std::endl;
        sensorVal = std::max(0.0f, std::min(sensorVal, 1.0f)); // clip
    }

    assert(!std::isnan(sensorVal) && sensorVal >= -0.01 && sensorVal <= 1.01);

    return sensorVal;
}

//---------------------------------------------------------------------------
Actions::Actions(
    PeepsPool& peepsPool,
    RandomUintGenerator& random,
    PheromoneSignals& pheromonSignal,
    const Grid& grid,
    const Parameters& params)
    : m_Params(params)
    , m_Random(random)
    , m_Signals(pheromonSignal)
    , m_Grid(grid)
    , m_PeepsPool(peepsPool)
{
}

//---------------------------------------------------------------------------
void Actions::executeActions(Peep &peep, unsigned simStep, std::array<float, eType::NUM_ACTIONS> &actionLevels)
{
    // Except eType::SET_RESPONSIVENESS all the action outputs, we'll apply an adjusted responsiveness
    // factor (see responseCurve() for more info). Range 0.0..1.0.
    float responsivenessAdjusted = AlgorithmHelpers::responseCurve(peep.responsiveness, m_Params);
    
    // Movement related variables.
    Coord offset{};
    // moveX,moveY will be the accumulators that will hold the sum of all the
    // urges to move along each axis. (+- floating values of arbitrary range).
    float moveX = 0.0;
    float moveY = 0.0;
    float level = 0.0;
    unsigned movementActionTypeCount = 0;

    // Check all the action listed in the available types. 
    // The list can be modified from outside the backend (i.e UI)
    for (size_t i = 0; i < m_AvailableTypes.size(); ++i)
    {   
        if (m_AvailableTypes[i] >= static_cast<unsigned>(Actions::eType::MOVE_NORTH) && 
            m_AvailableTypes[i] != static_cast<unsigned>(Actions::eType::NUM_ACTIONS))
            movementActionTypeCount++;

        // actionLevels index is representing the index of the type in m_AvailableTypes.
        const auto& type = m_AvailableTypes[i];
        level = actionLevels[i];
        switch (type)
        {
            // Responsiveness action - convert neuron action level from arbitrary float range
            // to the range 0.0..1.0. If this action neuron is enabled but not driven, will
            // default to mid-level 0.5.
            case Actions::eType::SET_RESPONSIVENESS:
            {
                level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
                peep.responsiveness = level;
                break;
            }
            // Oscillator period action - convert action level nonlinearly to
            // 2..4*p.stepsPerGeneration. If this action neuron is enabled but not driven,
            // will default to 1.5 + e^(3.5) = a period of 34 simSteps.
            case Actions::eType::SET_OSCILLATOR_PERIOD:
            {
                float newPeriodf01 = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
                unsigned newPeriod = 1 + (int)(1.5 + std::exp(7.0 * newPeriodf01));
                assert(newPeriod >= 2 && newPeriod <= 2048);
                peep.oscPeriod = newPeriod;
                break;
            }
            // Set longProbeDistance - convert action level to 1..maxLongProbeDistance.
            // If this action neuron is enabled but not driven, will default to
            // mid-level period of 17 simSteps.
            case Actions::eType::SET_LONGPROBE_DIST:
            {
                constexpr unsigned maxLongProbeDistance = 32;
                level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
                level = 1 + level * maxLongProbeDistance;
                peep.longProbeDist = (unsigned)level;
                break;
            }
            // Emit signal0 - if this action value is below a threshold, nothing emitted.
            // Otherwise convert the action value to a probability of emitting one unit of
            // signal (pheromone).
            // Pheromones may be emitted immediately (see signals.cpp). If this action neuron
            // is enabled but not driven, nothing will be emitted.
            case Actions::eType::EMIT_SIGNAL0:
            {
                constexpr float emitThreshold = 0.5;  // 0.0..1.0; 0.5 is midlevel
                level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
                level *= responsivenessAdjusted;
                if (level > emitThreshold && AlgorithmHelpers::prob2bool(level, m_Random)) {
                    m_Signals.increment(0, peep.loc);
                }
                break;
            }
            // Kill forward -- if this action value is > threshold, value is converted to probability
            // of an attempted murder. Probabilities under the threshold are considered 0.0.
            // If this action neuron is enabled but not driven, the neighbors are safe.
            case Actions::eType::KILL_FORWARD:
            {
                constexpr float killThreshold = 0.5;  // 0.0..1.0; 0.5 is midlevel
                level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
                level *= responsivenessAdjusted;
                if (level > killThreshold && AlgorithmHelpers::prob2bool((level - SensorsActions::ACTION_MIN) / SensorsActions::ACTION_RANGE, m_Random)) {
                    Coord otherLoc = peep.loc + peep.lastMoveDir;
                    if (m_Grid.isInBounds(otherLoc) && m_Grid.isOccupiedAt(otherLoc)) {
                        Peep &peep2 = m_PeepsPool.getPeep(otherLoc);
                        assert((peep.loc - peep2.loc).length() == 1);
                        m_PeepsPool.queueForDeath(peep2);
                    }
                }
                break;
            }
            // ------------- Planning related intranetwork action neurons ---------------

            // These actions neurons are used for planning, forecasting the future. They only used intranet.

            // Sets the planned location x to somewhere between 0 and sizeX.
            case Actions::eType::PlanPosX:
            {
                level = (std::tanh(level) + 1.0) / 2.0;
                level *= responsivenessAdjusted;
                peep.plannedLoc.x = level * m_Params.sizeX;
                break;
            }
            // Sets the planned location y to somewhere between 0 and sizeY.
            case Actions::eType::PlanPosY:
            {
                level = (std::tanh(level) + 1.0) / 2.0;
                level *= responsivenessAdjusted;
                peep.plannedLoc.y = level * m_Params.sizeY;
                break;
            }
            // Sets the planned planned time between the current sim step and the max sim step.
            case Actions::eType::PlanTime:
            {
                level = (std::tanh(level) + 1.0) / 2.0;
                level *= responsivenessAdjusted;
                peep.plannedSimStep = level * (m_Params.stepsPerGeneration - simStep) + simStep;
                peep.planTimeUpdateStep = simStep;
                break;
            }

            // ------------- Movement action neurons ---------------

            // There are multiple action neurons for movement. Each type of movement neuron
            // urges the individual to move in some specific direction. We sum up all the
            // X and Y components of all the movement urges, then pass the X and Y sums through
            // a transfer function (tanh()) to get a range -1.0..1.0. The absolute values of the
            // X and Y values are passed through prob2bool() to convert to -1, 0, or 1, then
            // multiplied by the component's signum. This results in the x and y components of
            // a normalized movement offset. I.e., the probability of movement in either
            // dimension is the absolute value of tanh of the action level X,Y components and
            // the direction is the sign of the X, Y components. For example, for a particular
            // action neuron:
            //     X, Y == -5.9, +0.3 as raw action levels received here
            //     X, Y == -0.999, +0.29 after passing raw values through tanh()
            //     Xprob, Yprob == 99.9%, 29% probability of X and Y becoming 1 (or -1)
            //     X, Y == -1, 0 after applying the sign and probability
            //     The agent will then be moved West (an offset of -1, 0) if it's a legal move.
            case Actions::eType::MOVE_EAST:
                moveX += (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_WEST:
                moveX -= (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_NORTH:
                moveY += (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_SOUTH:
                moveY -= (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_NE:
                moveX += (std::tanh(level) + 1.0) / 2.0;
                moveY += (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_SE:
                moveX += (std::tanh(level) + 1.0) / 2.0;
                moveY -= (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_SW:
                moveX -= (std::tanh(level) + 1.0) / 2.0;
                moveY -= (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_NW:
                moveX -= (std::tanh(level) + 1.0) / 2.0;
                moveY += (std::tanh(level) + 1.0) / 2.0;
                break;
            case Actions::eType::MOVE_RANDOM:
                offset = Dir::random8(m_Random).asNormalizedCoord();
                moveX += offset.x * (std::tanh(level) + 1.0) / 2.0;
                moveY += offset.y * (std::tanh(level) + 1.0) / 2.0;
                break;
            default:
              break;
        }
    }

    // Convert the accumulated X, Y sums to the range -1.0..1.0 and scale by the
    // peep's responsiveness (0.0..1.0) (adjusted by a curve)y
    moveX /= movementActionTypeCount;
    moveY /= movementActionTypeCount;
    moveX *= responsivenessAdjusted;
    moveY *= responsivenessAdjusted;

    // The probability of movement along each axis is the absolute value
    int16_t probX = (int16_t)AlgorithmHelpers::prob2bool(std::abs(moveX), m_Random); // convert abs(level) to 0 or 1
    int16_t probY = (int16_t)AlgorithmHelpers::prob2bool(std::abs(moveY), m_Random); // convert abs(level) to 0 or 1

    // The direction of movement (if any) along each axis is the sign
    int16_t signumX = moveX < 0.0 ? -1 : 1;
    int16_t signumY = moveY < 0.0 ? -1 : 1;

    // Generate a normalized movement offset, where each component is -1, 0, or 1
    Coord movementOffset = { (int16_t)(probX * signumX), (int16_t)(probY * signumY) };

    // Move there if it's a valid location
    Coord newLoc = peep.loc + movementOffset;
    if (m_Grid.isInBounds(newLoc) && m_Grid.isEmptyAt(newLoc)) {
        m_PeepsPool.queueForMove(peep, newLoc);
    }
}