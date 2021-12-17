#pragma once

#include "BasicTypes.h"
#include "Grid.h"
#include "Peep.h"

#include <cstdint>
#include <vector>

class Parameters;

// This class keeps track of alive and dead Indiv's and where they
// are in the Grid.
// Peeps allows spawning a live Indiv at a random or specific location
// in the grid, moving Indiv's from one grid location to another, and
// killing any Indiv.
// All the Indiv instances, living and dead, are stored in the private
// .peeps member. The .cull() function will remove dead members and
// replace their slots in the .peeps container with living members
// from the end of the container for compacting the container.
// Each Indiv has an identifying index in the range 1..0xfffe that is
// stored in the Grid at the location where the Indiv resides, such that
// a Grid element value n refers to .peeps[n]. Index value 0 is
// reserved, i.e., .peeps[0] is not a valid individual.
// This class does not manage properties inside Indiv except for the
// Indiv's location in the grid and its aliveness.
class PeepsPool {
public:
    PeepsPool(Grid& grid);

    void init(unsigned population, const Parameters& params);
    //! Safe to call during multithread mode.
    //! Indiv will remain alive and in-world until end of sim step when
    //! drainDeathQueue() is called.
    void queueForDeath(const Peep& peep);
    //! Called in single-thread mode at end of sim step. This executes all the
    //! queued deaths, removing the dead agents from the grid.
    void drainDeathQueue();
    //! Safe to call during multithread mode. Indiv won't move until end
    //! of sim step when drainMoveQueue() is called.
    void queueForMove(const Peep &, Coord newLoc);
    //! Called in single-thread mode at end of sim step. This executes all the
    //! queued movements. Each movement is typically one 8-neighbor cell distance
    //! but this function can move an individual any arbitrary distance.
    void drainMoveQueue();
    unsigned deathQueueSize() const { return deathQueue.size(); }
    // getPeep() does no error checking -- check first that loc is occupied
    Peep& getPeep(Coord loc) { return peeps[m_Grid.at(loc)]; }
    const Peep& getPeep(Coord loc) const { return peeps[m_Grid.at(loc)]; }
    // Direct access:
    Peep & operator[](uint16_t index) { return peeps[index]; }
    Peep const & operator[](uint16_t index) const { return peeps[index]; }
    
    void displaySampleGenomes(unsigned count);

private:
    std::vector<Peep> peeps; // Index value 0 is reserved
    std::vector<uint16_t> deathQueue;
    std::vector<std::pair<uint16_t, Coord>> moveQueue;

    Grid& m_Grid;
};