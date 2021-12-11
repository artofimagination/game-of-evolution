#pragma once

#include "BasicTypes.h"

#include <string>

class Grid;
class Parameters;
class Peep;
class PeepsPool;
class PheromoneSignals;

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


// Place the sensor neuron you want enabled prior to NUM_SENSES. Any
// that are after NUM_SENSES will be disabled in the simulator.
// If new items are added to this enum, also update the name functions
// in analysis.cpp.
// I means data about the individual, mainly stored in Indiv
// W means data about the environment, mainly stored in Peeps or Grid
enum Sensor {
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


// Place the action neuron you want enabled prior to NUM_ACTIONS. Any
// that are after NUM_ACTIONS will be disabled in the simulator.
// If new items are added to this enum, also update the name functions
// in analysis.cpp.
// I means the action affects the individual internally (Indiv)
// W means the action also affects the environment (Peeps or Grid)
enum Action {
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
    NUM_ACTIONS,       // <<----------------- END OF ACTIVE ACTIONS MARKER
    KILL_FORWARD,             // W
};

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

//! Returned sensor values range SENSOR_MIN..SENSOR_MAX.
float getSensor(    
    const Peep& peep,
    const PeepsPool& peeps,
    Sensor sensorNum,
    unsigned simStep,
    const Grid& grid,
    const Parameters& params,
    RandomUintGenerator& random,
    const PheromoneSignals& pheromoneSignals);

//! This converts sensor numbers to descriptive strings.
std::string sensorName(Sensor sensor);

//! This converts sensor numbers to mnemonic strings.
//! Useful for later processing by graph-nnet.py.
std::string sensorShortName(Sensor sensor);

//! Converts action numbers to mnemonic strings.
//! Useful for later processing by graph-nnet.py.
std::string actionShortName(Action action);

//! Print how many connections occur from each kind of sensor neuron and to
//! each kind of action neuron over the entire population. This helps us to
//! see which sensors and actions are most useful for survival.
void displaySensorActionReferenceCounts(const PeepsPool& peeps, const Parameters& params);

} // namespace SensorsActions