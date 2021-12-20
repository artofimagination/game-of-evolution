#pragma once

#include "BasicTypes.h"
#include "iBarriers.h"

namespace Barriers
{

class Rectangle : public iBarrier
{
public:
    Rectangle(const Coord& topleft, uint16_t width, uint16_t height);

    struct Setup
    {
        Coord topLeft{};
        uint16_t width;
        uint16_t height;
    };

    void Draw(uint16_t barrierMask, const Parameters& params, Grid& grid) override;

    Setup& GetSetup() { return m_Setup; };
private:
    Setup m_Setup;
};

} // namespace Barriers