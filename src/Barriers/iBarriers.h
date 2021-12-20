#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class Grid;
class Parameters;
class RandomUintGenerator;

enum class eBarrierType : uint8_t
{
    NoBarrier,
    VerticalBarConstantLoc,
    VerticalBarRandomLoc,
    FiveBlocksStaggered,
    HorizontalBarConstantLoc,
    ThreeFloatingIslands,
    SpotsSpecified,
    NoOfTypes
};

namespace Barriers
{

class iBarrier
{
public:
  virtual void Draw(uint16_t barrierMask, const Parameters& params, Grid& grid) = 0;
};

//! Sets the content of \a barriers with the appropriate barrier types.
void createBarrier(
    eBarrierType barrierType,
    std::vector<std::unique_ptr<iBarrier> >& barriers,
    RandomUintGenerator& random,
    const Parameters& params);

} // namespace Barriers