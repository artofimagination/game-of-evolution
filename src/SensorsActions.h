#pragma once

#include "BasicTypes.h"

#include <array>
#include <string>
#include <vector>

class Grid;
class Parameters;
class Peep;
class PeepsPool;
class PheromoneSignals;
class RandomUintGenerator;

class Sensors
{
public:
    // Place the sensor neuron you want enabled prior to NUM_SENSES. Any
    // that are after NUM_SENSES will be disabled in the simulator.
    // If new items are added to this enum, also update the name functions
    // in analysis.cpp.
    // I means data about the individual, mainly stored in Indiv
    // W means data about the environment, mainly stored in Peeps or Grid
    enum eType {
        LOC_X,             // I distance from left edge
        LOC_Y,             // I distance from bottom
        BOUNDARY_DIST_X,   // I X distance to nearest edge of world
        BOUNDARY_DIST,     // I distance to nearest edge of world
        BOUNDARY_DIST_Y,   // I Y distance to nearest edge of world
        GENETIC_SIM_FWD,   // I genetic similarity forward
        LAST_MOVE_DIR_X,   // I +- amount of X movement in last movement
        LAST_MOVE_DIR_Y,   // I +- amount of Y movement in last movement
        LONGPROBE_POP_FWD, // W long look for population forward
        LONGPROBE_BAR_FWD, // W long look for barriers forward
        POPULATION,        // W population density in neighborhood
        POPULATION_FWD,    // W population density in the forward-reverse axis
        POPULATION_LR,     // W population density in the left-right axis
        OSC1,              // I oscillator +-value
        AGE,               // I
        BARRIER_FWD,       // W neighborhood barrier distance forward-reverse axis
        BARRIER_LR,        // W neighborhood barrier distance left-right axis
        RANDOM,            //   random sensor value, uniform distribution
        SIGNAL0,           // W strength of signal0 in neighborhood
        SIGNAL0_FWD,       // W strength of signal0 in the forward-reverse axis
        SIGNAL0_LR,        // W strength of signal0 in the left-right axis
        NUM_SENSES,        // <<------------------ END OF ACTIVE SENSES MARKER
    };

    //! Updates the available sensor types vector.
    void UpdateAvailableSensorTypes(const std::vector<eType>& types) { m_AvailableTypes = types; }
    //! Returns the available sensor type count.
    unsigned AvailableSensorTypeCount() const { return m_AvailableTypes.size(); }

    //! Returned sensor values range SENSOR_MIN..SENSOR_MAX.
    float getSensor(    
        const Peep& peep,
        const PeepsPool& peeps,
        uint8_t sensorTypeIndex,
        unsigned simStep,
        const Grid& grid,
        const Parameters& params,
        RandomUintGenerator& random,
        const PheromoneSignals& pheromoneSignals) const;

private:
    std::vector<eType> m_AvailableTypes{};         ///!< Contains the available sensors types.
};

class Actions
{
public:
    // Place the action neuron you want enabled prior to NUM_ACTIONS. Any
    // that are after NUM_ACTIONS will be disabled in the simulator.
    // If new items are added to this enum, also update the name functions
    // in analysis.cpp.
    // I means the action affects the peep internally (Peep)
    // W means the action also affects the environment (Peeps or Grid)
    enum eType {
        MOVE_X,                   // W +- X component of movement
        MOVE_Y,                   // W +- Y component of movement
        MOVE_FORWARD,             // W continue last direction
        MOVE_RL,                  // W +- component of movement
        MOVE_RANDOM,              // W
        SET_OSCILLATOR_PERIOD,    // I
        SET_LONGPROBE_DIST,       // I
        SET_RESPONSIVENESS,       // I
        EMIT_SIGNAL0,             // W
        MOVE_EAST,                // W
        MOVE_WEST,                // W
        MOVE_NORTH,               // W
        MOVE_SOUTH,               // W
        MOVE_LEFT,                // W
        MOVE_RIGHT,               // W
        MOVE_REVERSE,             // W
        KILL_FORWARD,             // W
        NUM_ACTIONS,              // <<----------------- END OF ACTIVE ACTIONS MARKER
    };

    Actions(
        PeepsPool& peepsPool,
        RandomUintGenerator& random,
        PheromoneSignals& pheromonSignal,
        const Grid& grid,
        const Parameters& params);

    //! Updates the available action types vector.
    void UpdateAvailableActionTypes(const std::vector<eType>& types) { m_AvailableTypes = types; }
    //! Returns the available action type count.
    unsigned AvailableActionTypeCount() const { return m_AvailableTypes.size(); }
    //! Returns the available action types.
    const std::vector<eType>& AvailableTypes() const { return m_AvailableTypes; }
    /**********************************************************************************
    Action levels are driven by sensors or internal neurons as connected by an agent's
    neural net brain. Each agent's neural net is reevaluated once each simulator
    step (simStep). After evaluating the action neuron outputs, this function is
    called to execute the actions according to their output levels. This function is
    called in multi-threaded mode and operates on a single individual while other
    threads are doing to the same to other peeps.

    Action (their output) values arrive here as floating point values of arbitrary
    range (because they are the raw sums of zero or more weighted inputs) and will
    eventually be converted in this function to a probability 0.0..1.0 of actually
    getting executed.

    For the various possible action neurons, if they are driven by a sufficiently
    strong level, we do this:

        MOVE_* actions- queue our agent for deferred movement with peeps.queueForMove(); the
            queue will be executed at the end of the multithreaded loop in a single thread.
        SET_RESPONSIVENESS action - immediately change indiv.responsiveness to the action
            level scaled to 0.0..1.0 (because we have exclusive access to this member in
            our own individual during this function)
        SET_OSCILLATOR_PERIOD action - immediately change our individual's indiv.oscPeriod
            to the action level exponentially scaled to 2..2048 (TBD)
        EMIT_SIGNALn action(s) - immediately increment the signal level at our agent's
            location using signals.increment() (using a thread-safe call)
        KILL_FORWARD action - queue the other agent for deferred death with
            peeps.queueForDeath()

    The deferred movement and death queues will be emptied by the caller at the end of the
    simulator step by endOfSimStep() in a single thread after all peeps have been
    evaluated multithreadedly.
    **********************************************************************************/
    void executeActions(Peep &peep, std::array<float, eType::NUM_ACTIONS> &actionLevels);

private:
    std::vector<eType> m_AvailableTypes{};         ///!< Contains the available action types.
    const Parameters& m_Params;
    RandomUintGenerator& m_Random;
    PheromoneSignals& m_Signals;
    const Grid& m_Grid;
    PeepsPool& m_PeepsPool;
};

namespace SensorsActions
{

// Neuron Sources (Sensors) and Sinks (Actions)

// These sensor, neuron, and action value ranges are here for documentation
// purposes. Most functions now assume these ranges. We no longer support changes
// to these ranges.
constexpr float SENSOR_MIN = 0.0;
constexpr float SENSOR_MAX = 1.0;
constexpr float SENSOR_RANGE = SENSOR_MAX - SENSOR_MIN;

constexpr float NEURON_MIN = -1.0;
constexpr float NEURON_MAX = 1.0;
constexpr float NEURON_RANGE = NEURON_MAX - NEURON_MIN;

constexpr float ACTION_MIN = 0.0;
constexpr float ACTION_MAX = 1.0;
constexpr float ACTION_RANGE = ACTION_MAX - ACTION_MIN;

//! returns magnitude of the specified signal layer in a neighborhood, with
//! 0.0..maxSignalSum converted to the sensor range.
float getSignalDensity(unsigned layerNum, Coord loc, const PheromoneSignals& pheromoneSignals, const Parameters& params);

//! Converts the signal density along the specified axis to sensor range. The
//! values of cell signal levels are scaled by the inverse of their distance times
//! the positive absolute cosine of the difference of their angle and the
//! specified axis. The maximum positive or negative magnitude of the sum is
//! about 2*radius*SIGNAL_MAX (?). We don't adjust for being close to a border,
//! so signal densities along borders and in corners are commonly sparser than
//! away from borders.
float getSignalDensityAlongAxis(unsigned layerNum, Coord loc, Dir dir, const PheromoneSignals& pheromoneSignals, const Parameters& params);

//! This converts sensor numbers to descriptive strings.
std::string sensorName(Sensors::eType sensor);

//! This converts sensor numbers to mnemonic strings.
//! Useful for later processing by graph-nnet.py.
std::string sensorShortName(Sensors::eType sensor);

//! Converts action numbers to mnemonic strings.
//! Useful for later processing by graph-nnet.py.
std::string actionShortName(Actions::eType action);

//! Converts action numbers to descriptive strings.
std::string actionName(Actions::eType action);

//! Print how many connections occur from each kind of sensor neuron and to
//! each kind of action neuron over the entire population. This helps us to
//! see which sensors and actions are most useful for survival.
void displaySensorActionReferenceCounts(const PeepsPool& peeps, const Parameters& params);

} // namespace SensorsActions