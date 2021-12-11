#include "PheromoneSignals.h"

#include "Parameters.h"
#include "AlgorithmHelpers.h"

#include "limits.h"

//-------------------------------------------------------------------------
PheromoneSignals::PheromoneSignals(const Parameters& params)
    : m_Params(params)
{

}

//-------------------------------------------------------------------------
void PheromoneSignals::init(uint16_t numLayers, uint16_t sizeX, uint16_t sizeY)
{
    data = std::vector<Layer>(numLayers, Layer(sizeX, sizeY));
}

//-------------------------------------------------------------------------
void PheromoneSignals::fade(unsigned layerNum)
{
    constexpr unsigned fadeAmount = 1;

    for (int16_t x = 0; x < m_Params.sizeX; ++x) {
        for (int16_t y = 0; y < m_Params.sizeY; ++y) {
            if ((*this)[layerNum][x][y] >= fadeAmount) {
                (*this)[layerNum][x][y] -= fadeAmount;  // fade center cell
            } else {
                (*this)[layerNum][x][y] = 0;
            }
        }
    }
}

// Increases the specified location by centerIncreaseAmount,
// and increases the neighboring cells by neighborIncreaseAmount

// Is it ok that multiple readers are reading this container while
// this single thread is writing to it?  todo!!!
//-------------------------------------------------------------------------
void PheromoneSignals::increment(uint16_t layerNum, Coord loc)
{
    constexpr float radius = 1.5;
    constexpr uint8_t centerIncreaseAmount = 2;
    constexpr uint8_t neighborIncreaseAmount = 1;

#pragma omp critical
    {
        AlgorithmHelpers::visitNeighborhood(loc, radius, [layerNum, this](Coord loc) {
            if ((*this)[layerNum][loc.x][loc.y] < SIGNAL_MAX) {
                (*this)[layerNum][loc.x][loc.y] =
                        std::min<unsigned>(SIGNAL_MAX,
                                           (*this)[layerNum][loc.x][loc.y] + neighborIncreaseAmount);
            }
        }, m_Params);

        if ((*this)[layerNum][loc.x][loc.y] < SIGNAL_MAX) {
            (*this)[layerNum][loc.x][loc.y] =
                        std::min<unsigned>(SIGNAL_MAX,
                                           (*this)[layerNum][loc.x][loc.y] + centerIncreaseAmount);
        }
    }
}