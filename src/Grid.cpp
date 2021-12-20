#include "Grid.h"

#include "AlgorithmHelpers.h"
#include "Barriers/iBarriers.h"
#include "Barriers/CircleBarrier.h"
#include "Parameters.h"

#include <cassert>

constexpr uint16_t EMPTY = 0; // Index value 0 is reserved
constexpr uint16_t BARRIER = 0xffff;

//-------------------------------------------------------------------------
Grid::Grid(const Parameters& params, RandomUintGenerator& r)
    : m_Params(params)
    , m_RandomGenerator(r)
{

}

//-------------------------------------------------------------------------
void Grid::init()
{
    auto col = Column(m_Params.sizeY);
    data = std::vector<Column>(m_Params.sizeX, col);
}

//-------------------------------------------------------------------------
bool Grid::isEmptyAt(Coord loc) const 
{ 
    return at(loc) == EMPTY; 
}

//-------------------------------------------------------------------------
bool Grid::isOccupiedAt(Coord loc) const 
{ 
    return at(loc) != EMPTY && at(loc) != BARRIER; 
}

//-------------------------------------------------------------------------
bool Grid::isBarrierAt(Coord loc) const 
{ 
    return at(loc) == BARRIER; 
}

//-------------------------------------------------------------------------
void Grid::createBarrier(eBarrierType barrierType, std::vector<std::unique_ptr<Barriers::iBarrier> >& barriers)
{
    Barriers::createBarrier(barrierType, barriers, m_RandomGenerator, m_Params);
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        barriers[i]->Draw(BARRIER, m_Params, *this);
        switch (barrierType)
        {
          case eBarrierType::SpotsSpecified:
          case eBarrierType::ThreeFloatingIslands:
              barrierCenters.push_back(static_cast<Barriers::Circle*>(barriers[i].get())->GetSetup().center);
              break;
          case eBarrierType::VerticalBarConstantLoc:
          case eBarrierType::VerticalBarRandomLoc:
          case eBarrierType::NoOfTypes:
          case eBarrierType::NoBarrier:
          case eBarrierType::FiveBlocksStaggered:
          default:
              break;
        }
    }
}

//-------------------------------------------------------------------------
Coord Grid::findEmptyLocation() const {
    Coord loc;
    bool found = false;

    while (!found) {
        loc.x = m_RandomGenerator(0, m_Params.sizeX - 1);
        loc.y = m_RandomGenerator(0, m_Params.sizeY - 1);
        if (isEmptyAt(loc)) {
            break;
        }
    }
    return loc;
}