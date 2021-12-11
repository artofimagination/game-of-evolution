#pragma once

#include "BasicTypes.h"

#include <cstdint>
#include <functional>
#include <vector>

class Parameters;
class RandomUintGenerator;

// Grid is a somewhat dumb 2D container of unsigned 16-bit values.
// Grid understands that the elements are either EMPTY, BARRIER, or
// otherwise an index value into the peeps container.
// The elements are allocated and cleared to EMPTY in the ctor.
// Prefer .at() and .set() for random element access. Or use Grid[x][y]
// for direct access where the y index is the inner loop.
// Element values are not otherwise interpreted by class Grid.
class Grid {
public:
    // Column order here allows us to access grid elements as data[x][y]
    // while thinking of x as column and y as row
    struct Column {
        Column(uint16_t numRows) : data { std::vector<uint16_t>(numRows, 0) } { }
        void zeroFill() { std::fill(data.begin(), data.end(), 0); }
        uint16_t& operator[](uint16_t rowNum) { return data[rowNum]; }
        uint16_t operator[](uint16_t rowNum) const { return data[rowNum]; }
        size_t size() const { return data.size(); }
    private:
        std::vector<uint16_t> data;
    };

    Grid(const Parameters& params, RandomUintGenerator& r);

    //! Allocates space for the 2D grid
    void init();
    void zeroFill() { for (Column &column : data) column.zeroFill(); }
    uint16_t sizeX() const { return data.size(); }
    uint16_t sizeY() const { return data[0].size(); }
    bool isInBounds(Coord loc) const { return loc.x >= 0 && loc.x < sizeX() && loc.y >= 0 && loc.y < sizeY(); }
    bool isEmptyAt(Coord loc) const;
    bool isBarrierAt(Coord loc) const;
    // Occupied means an agent is living there.
    bool isOccupiedAt(Coord loc) const;
    bool isBorder(Coord loc) const { return loc.x == 0 || loc.x == sizeX() - 1 || loc.y == 0 || loc.y == sizeY() - 1; }
    uint16_t at(Coord loc) const { return data[loc.x][loc.y]; };
    uint16_t at(uint16_t x, uint16_t y) const { return data[x][y]; }

    void set(Coord loc, uint16_t val) { data[loc.x][loc.y] = val; }
    void set(uint16_t x, uint16_t y, uint16_t val) { data[x][y] = val; }
    //! Finds a random unoccupied location in the grid.
    Coord findEmptyLocation() const;

    // This generates barrier points, which are grid locations with value
    // BARRIER. A list of barrier locations is saved in private member
    // Grid::barrierLocations and, for some scenarios, Grid::barrierCenters.
    // Those members are available read-only with Grid::getBarrierLocations().
    // This function assumes an empty grid. This is typically called by
    // the main simulator thread after Grid::init() or Grid::zeroFill().

    // This file typically is under constant development and change for
    // specific scenarios.
    void createBarrier(unsigned barrierType);
    const std::vector<Coord> &getBarrierLocations() const { return barrierLocations; }
    const std::vector<Coord> &getBarrierCenters() const { return barrierCenters; }
    // Direct access:
    Column & operator[](uint16_t columnXNum) { return data[columnXNum]; }
    const Column & operator[](uint16_t columnXNum) const { return data[columnXNum]; }
private:
    const Parameters& m_Params;
    RandomUintGenerator& m_RandomGenerator;

    std::vector<Column> data;
    std::vector<Coord> barrierLocations;
    std::vector<Coord> barrierCenters;
};