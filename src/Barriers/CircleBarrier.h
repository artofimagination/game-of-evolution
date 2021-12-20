#pragma once

#include "BasicTypes.h"
#include "iBarriers.h"

namespace Barriers
{

class Circle : public iBarrier
{
public:
    Circle(const Coord& center, float radius);

    struct Setup
    {
        Coord center{};
        float radius;
    };

    void Draw(uint16_t barrierMask, const Parameters& params, Grid& grid) override;

    Setup& GetSetup() { return m_Setup; };
private:
    Setup m_Setup;
};

} // namespace Barriers