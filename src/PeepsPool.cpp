#include "PeepsPool.h"

//-------------------------------------------------------------------------
PeepsPool::PeepsPool(Grid& grid)
  : m_Grid(grid)
{

}

//-------------------------------------------------------------------------
void PeepsPool::init(unsigned population, const Parameters& params)
{
    // Index 0 is reserved, so add one:
    for(unsigned i = 0; i < population + 1; ++i)
    {
        peeps.emplace_back(Peep(params, m_Grid));
    }
}

//-------------------------------------------------------------------------
void PeepsPool::queueForDeath(const Peep &peep)
{
    #pragma omp critical
    {
        if (peep.alive)
            deathQueue.push_back(peep.index);
    }
}

//-------------------------------------------------------------------------
void PeepsPool::drainDeathQueue()
{
    for (uint16_t index : deathQueue) {
        auto& peep = (*this)[index];
        m_Grid.set(peep.loc, 0);
        peep.alive = false;
    }
    deathQueue.clear();
}

//-------------------------------------------------------------------------
void PeepsPool::drainMoveQueue()
{
    for (auto& moveRecord : moveQueue) {
        auto& peep = (*this)[moveRecord.first];
        Coord newLoc = moveRecord.second;
        Dir moveDir = (newLoc - peep.loc).asDir();
        if (m_Grid.isEmptyAt(newLoc)) {
            m_Grid.set(peep.loc, 0);
            m_Grid.set(newLoc, peep.index);
            peep.loc = newLoc;
            peep.lastMoveDir = moveDir;
        }
    }
    moveQueue.clear();
}

//-------------------------------------------------------------------------
void PeepsPool::queueForMove(const Peep &peep, Coord newLoc)
{
    #pragma omp critical
    {
        auto record = std::make_pair<uint16_t, Coord>(uint16_t(peep.index), Coord(newLoc));
        moveQueue.push_back(record);
    }
}