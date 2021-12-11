#pragma once

#include "BasicTypes.h"

#include <functional>

class Parameters;
class Grid;
class Peep;
class RandomUintGenerator;

namespace AlgorithmHelpers
{

//! This is a utility function used when inspecting a local neighborhood around
//! some location. This function feeds each valid (in-bounds) location in the specified
//! neighborhood to the specified function. Locations include self (center of the neighborhood).
void visitNeighborhood(Coord loc, float radius, std::function<void(Coord)> f, const Parameters& params);

//! Converts the population along the specified axis to the sensor range. The
//! locations of neighbors are scaled by the inverse of their distance times
//! the positive absolute cosine of the difference of their angle and the
//! specified axis. The maximum positive or negative magnitude of the sum is
//! about 2*radius. We don't adjust for being close to a border, so populations
//! along borders and in corners are commonly sparser than away from borders.
//! An empty neighborhood results in a sensor value exactly midrange; below
//! midrange if the population density is greatest in the reverse direction,
//! above midrange if density is greatest in forward direction.
float getPopulationDensityAlongAxis(Coord loc, Dir dir, const Grid& grid, const Parameters& params);

//! Converts the number of locations (not including loc) to the next barrier location
//! along opposite directions of the specified axis to the sensor range. If no barriers
//! are found, the result is sensor mid-range. Ignores agents in the path.
float getShortProbeBarrierDistance(Coord loc0, Dir dir, unsigned probeDistance, const Grid& grid);

//! Returns the number of locations to the next agent in the specified
//! direction, not including loc. If the probe encounters a boundary or a
//! barrier before reaching the longProbeDist distance, returns longProbeDist.
//! Returns 0..longProbeDist.
unsigned LongProbePopulationFwd(const Peep& peep, const Grid& grid);

//! Returns the number of locations to the next barrier in the
//! specified direction, not including loc. Ignores agents in the way.
//! If the distance to the border is less than the longProbeDist distance
//! and no barriers are found, returns longProbeDist.
//! Returns 0..longProbeDist.
unsigned LongProbeBarrierFwd(const Peep& peep, const Grid& grid);

//! This takes a probability from 0.0..1.0 and adjusts it according to an
//! exponential curve. The steepness of the curve is determined by the K factor
//! which is a small positive integer. This tends to reduce the activity level
//! a bit (makes the peeps less reactive and jittery).
float responseCurve(float r, const Parameters& params);

//! Given a factor in the range 0.0..1.0, return a bool with the
//! probability of it being true proportional to factor. For example, if
//! factor == 0.2, then there is a 20% chance this function will
//! return true.
bool prob2bool(float factor, RandomUintGenerator& random);

} // namespace AlgorithmHelpers