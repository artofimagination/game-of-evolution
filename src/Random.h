#pragma once

#include <cstdint>

constexpr uint32_t RANDOM_UINT_MAX = 0xffffffff;

class RandomUintGenerator{
private:
    // for Marsaglia
    uint32_t rngx;
    uint32_t rngy;
    uint32_t rngz;
    uint32_t rngc;
    // for Jenkins
    uint32_t a, b, c, d;
public:
    RandomUintGenerator(bool deterministic = false);
    RandomUintGenerator& operator=(const RandomUintGenerator &rhs) = default;
    void randomize();
    uint32_t operator()();
    unsigned operator()(unsigned min, unsigned max);
};
