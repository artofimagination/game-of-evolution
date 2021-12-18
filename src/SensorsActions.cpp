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
std::string sensorName(Sensor sensor)
{
    switch(sensor) {
    case AGE: return "age"; break;
    case BOUNDARY_DIST: return "boundary dist"; break;
    case BOUNDARY_DIST_X: return "boundary dist X"; break;
    case BOUNDARY_DIST_Y: return "boundary dist Y"; break;
    case LAST_MOVE_DIR_X: return "last move dir X"; break;
    case LAST_MOVE_DIR_Y: return "last move dir Y"; break;
    case LOC_X: return "loc X"; break;
    case LOC_Y: return "loc Y"; break;
    case LONGPROBE_POP_FWD: return "long probe population fwd"; break;
    case LONGPROBE_BAR_FWD: return "long probe barrier fwd"; break;
    case BARRIER_FWD: return "short probe barrier fwd-rev"; break;
    case BARRIER_LR: return "short probe barrier left-right"; break;
    case OSC1: return "osc1"; break;
    case POPULATION: return "population"; break;
    case POPULATION_FWD: return "population fwd"; break;
    case POPULATION_LR: return "population LR"; break;
    case RANDOM: return "random"; break;
    case SIGNAL0: return "signal 0"; break;
    case SIGNAL0_FWD: return "signal 0 fwd"; break;
    case SIGNAL0_LR: return "signal 0 LR"; break;
    case GENETIC_SIM_FWD: return "genetic similarity fwd"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
std::string sensorShortName(Sensor sensor)
{
    switch(sensor) {
    case AGE: return "Age"; break;
    case BOUNDARY_DIST: return "ED"; break;
    case BOUNDARY_DIST_X: return "EDx"; break;
    case BOUNDARY_DIST_Y: return "EDy"; break;
    case LAST_MOVE_DIR_X: return "LMx"; break;
    case LAST_MOVE_DIR_Y: return "LMy"; break;
    case LOC_X: return "Lx"; break;
    case LOC_Y: return "Ly"; break;
    case LONGPROBE_POP_FWD: return "LPf"; break;
    case LONGPROBE_BAR_FWD: return "LPb"; break;
    case BARRIER_FWD: return "Bfd"; break;
    case BARRIER_LR: return "Blr"; break;
    case OSC1: return "Osc"; break;
    case POPULATION: return "Pop"; break;
    case POPULATION_FWD: return "Pfd"; break;
    case POPULATION_LR: return "Plr"; break;
    case RANDOM: return "Rnd"; break;
    case SIGNAL0: return "Sg"; break;
    case SIGNAL0_FWD: return "Sfd"; break;
    case SIGNAL0_LR: return "Slr"; break;
    case GENETIC_SIM_FWD: return "Gen"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
std::string actionShortName(Action action)
{
    switch(action) {
    case MOVE_EAST: return "MvE"; break;
    case MOVE_WEST: return "MvW"; break;
    case MOVE_NORTH: return "MvN"; break;
    case MOVE_SOUTH: return "MvS"; break;
    case MOVE_X: return "MvX"; break;
    case MOVE_Y: return "MvY"; break;
    case MOVE_FORWARD: return "Mfd"; break;
    case SET_RESPONSIVENESS: return "Res"; break;
    case SET_OSCILLATOR_PERIOD: return "OSC"; break;
    case EMIT_SIGNAL0: return "SG"; break;
    case KILL_FORWARD: return "Klf"; break;
    case MOVE_REVERSE: return "Mrv"; break;
    case MOVE_LEFT: return "MvL"; break;
    case MOVE_RIGHT: return "MvR"; break;
    case MOVE_RL: return "MRL"; break;
    case MOVE_RANDOM: return "Mrn"; break;
    case SET_LONGPROBE_DIST: return "LPD"; break;
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
float getSensor(
    const Peep& peep,
    const PeepsPool& peeps,
    Sensor sensorNum,
    unsigned simStep,
    const Grid& grid,
    const Parameters& params,
    RandomUintGenerator& random,
    const PheromoneSignals& pheromoneSignals)
{
    float sensorVal = 0.0;

    switch (sensorNum) {
    case Sensor::AGE:
        // Converts age (units of simSteps compared to life expectancy)
        // linearly to normalized sensor range 0.0..1.0
        sensorVal = (float)peep.age / params.stepsPerGeneration;
        break;
    case Sensor::BOUNDARY_DIST:
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
    case Sensor::BOUNDARY_DIST_X:
    {
        // Measures the distance to nearest boundary in the east-west axis,
        // max distance is half the grid width; scaled to sensor range 0.0..1.0.
        int minDistX = std::min<int>(peep.loc.x, (params.sizeX - peep.loc.x) - 1);
        sensorVal = minDistX / (params.sizeX / 2.0);
        break;
    }
    case Sensor::BOUNDARY_DIST_Y:
    {
        // Measures the distance to nearest boundary in the south-north axis,
        // max distance is half the grid height; scaled to sensor range 0.0..1.0.
        int minDistY = std::min<int>(peep.loc.y, (params.sizeY - peep.loc.y) - 1);
        sensorVal = minDistY / (params.sizeY / 2.0);
        break;
    }
    case Sensor::LAST_MOVE_DIR_X:
    {
        // X component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastX = peep.lastMoveDir.asNormalizedCoord().x;
        sensorVal = lastX == 0 ? 0.5 :
                (lastX == -1 ? 0.0 : 1.0);
        break;
    }
    case Sensor::LAST_MOVE_DIR_Y:
    {
        // Y component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastY = peep.lastMoveDir.asNormalizedCoord().y;
        sensorVal = lastY == 0 ? 0.5 :
                (lastY == -1 ? 0.0 : 1.0);
        break;
    }
    case Sensor::LOC_X:
        // Maps current X location 0..p.sizeX-1 to sensor range 0.0..1.0
        sensorVal = (float)peep.loc.x / (params.sizeX - 1);
        break;
    case Sensor::LOC_Y:
        // Maps current Y location 0..p.sizeY-1 to sensor range 0.0..1.0
        sensorVal = (float)peep.loc.y / (params.sizeY - 1);
        break;
    case Sensor::OSC1:
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
    case Sensor::LONGPROBE_POP_FWD:
    {
        // Measures the distance to the nearest other individual in the
        // forward direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = AlgorithmHelpers::LongProbePopulationFwd(peep, grid) / (float)peep.longProbeDist; // 0..1
        break;
    }
    case Sensor::LONGPROBE_BAR_FWD:
    {
        // Measures the distance to the nearest barrier in the forward
        // direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = AlgorithmHelpers::LongProbeBarrierFwd(peep, grid) / (float)peep.longProbeDist; // 0..1
        break;
    }
    case Sensor::POPULATION:
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
    case Sensor::POPULATION_FWD:
        // Sense population density along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getPopulationDensityAlongAxis(peep.loc, peep.lastMoveDir, grid, params);
        break;
    case Sensor::POPULATION_LR:
        // Sense population density along an axis 90 degrees from last movement direction
        sensorVal = AlgorithmHelpers::getPopulationDensityAlongAxis(peep.loc, peep.lastMoveDir.rotate90DegCW(), grid, params);
        break;
    case Sensor::BARRIER_FWD:
        // Sense the nearest barrier along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getShortProbeBarrierDistance(peep.loc, peep.lastMoveDir, params.shortProbeBarrierDistance, grid);
        break;
    case Sensor::BARRIER_LR:
        // Sense the nearest barrier along axis perpendicular to last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = AlgorithmHelpers::getShortProbeBarrierDistance(peep.loc, peep.lastMoveDir.rotate90DegCW(), params.shortProbeBarrierDistance, grid);
        break;
    case Sensor::RANDOM:
        // Returns a random sensor value in the range 0.0..1.0.
        sensorVal = random() / (float)UINT_MAX;
        break;
    case Sensor::SIGNAL0:
        // Returns magnitude of signal0 in the local neighborhood, with
        // 0.0..maxSignalSum converted to sensorRange 0.0..1.0
        sensorVal = getSignalDensity(0, peep.loc, pheromoneSignals, params);
        break;
    case Sensor::SIGNAL0_FWD:
        // Sense signal0 density along axis of last movement direction
        sensorVal = getSignalDensityAlongAxis(0, peep.loc, peep.lastMoveDir, pheromoneSignals, params);
        break;
    case Sensor::SIGNAL0_LR:
        // Sense signal0 density along an axis perpendicular to last movement direction
        sensorVal = getSignalDensityAlongAxis(0, peep.loc, peep.lastMoveDir.rotate90DegCW(), pheromoneSignals, params);
        break;
    case Sensor::GENETIC_SIM_FWD:
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
    default:
        assert(false);
        break;
    }

    if (std::isnan(sensorVal) || sensorVal < -0.01 || sensorVal > 1.01) {
        std::cout << "sensorVal=" << (int)sensorVal << " for " << sensorName((Sensor)sensorNum) << std::endl;
        sensorVal = std::max(0.0f, std::min(sensorVal, 1.0f)); // clip
    }

    assert(!std::isnan(sensorVal) && sensorVal >= -0.01 && sensorVal <= 1.01);

    return sensorVal;
}

//-------------------------------------------------------------------------
//! Converts action numbers to descriptive strings.
//-------------------------------------------------------------------------
std::string actionName(Action action)
{
    switch(action) {
    case MOVE_EAST: return "move east"; break;
    case MOVE_WEST: return "move west"; break;
    case MOVE_NORTH: return "move north"; break;
    case MOVE_SOUTH: return "move south"; break;
    case MOVE_FORWARD: return "move fwd"; break;
    case MOVE_X: return "move X"; break;
    case MOVE_Y: return "move Y"; break;
    case SET_RESPONSIVENESS: return "set inv-responsiveness"; break;
    case SET_OSCILLATOR_PERIOD: return "set osc1"; break;
    case EMIT_SIGNAL0: return "emit signal 0"; break;
    case KILL_FORWARD: return "kill fwd"; break;
    case MOVE_REVERSE: return "move reverse"; break;
    case MOVE_LEFT: return "move left"; break;
    case MOVE_RIGHT: return "move right"; break;
    case MOVE_RL: return "move R-L"; break;
    case MOVE_RANDOM: return "move random"; break;
    case SET_LONGPROBE_DIST: return "set longprobe dist"; break;
    default: assert(false); break;
    }
}

//-------------------------------------------------------------------------
void displaySensorActionReferenceCounts(const PeepsPool& peeps, const Parameters& params)
{
    std::vector<unsigned> sensorCounts(Sensor::NUM_SENSES, 0);
    std::vector<unsigned> actionCounts(Action::NUM_ACTIONS, 0);

    for (unsigned index = 1; index <= params.population; ++index) {
        if (peeps[index].alive) {
            const Peep &peep = peeps[index];
            for (const auto& gene : peep.nnet.connections) {
                if (gene.sourceType == Genetics::SENSOR) {
                    assert(gene.sourceNum < Sensor::NUM_SENSES);
                    ++sensorCounts[(Sensor)gene.sourceNum];
                }
                if (gene.sinkType == Genetics::ACTION) {
                    assert(gene.sinkNum < Action::NUM_ACTIONS);
                    ++actionCounts[(Action)gene.sinkNum];
                }
            }
        }
    }

    std::cout << "Sensors in use:" << std::endl;
    for (unsigned i = 0; i < sensorCounts.size(); ++i) {
        if (sensorCounts[i] > 0) {
            std::cout << "  " << sensorCounts[i] << " - " << sensorName((Sensor)i) << std::endl;
        }
    }
    std::cout << "Actions in use:" << std::endl;
    for (unsigned i = 0; i < actionCounts.size(); ++i) {
        if (actionCounts[i] > 0) {
            std::cout << "  " << actionCounts[i] << " - " << actionName((Action)i) << std::endl;
        }
    }
}

} // namespace SensorsActions