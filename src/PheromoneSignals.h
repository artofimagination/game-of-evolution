#pragma once

#include "BasicTypes.h"

#include <cstdint>
#include <vector>

class Parameters;

constexpr unsigned SIGNAL_MIN = 0;
constexpr unsigned SIGNAL_MAX = UINT8_MAX;

struct PheromoneSignals 
{
    PheromoneSignals(const Parameters& params);

    struct Column {
        Column(uint16_t numRows) : data { std::vector<uint8_t>(numRows, 0) } { }
        uint8_t& operator[](uint16_t rowNum) { return data[rowNum]; }
        uint8_t operator[](uint16_t rowNum) const { return data[rowNum]; }
        void zeroFill() { std::fill(data.begin(), data.end(), 0); }
    private:
        std::vector<uint8_t> data;
    };

    struct Layer {
        Layer(uint16_t numCols, uint16_t numRows) : data { std::vector<Column>(numCols, Column(numRows)) } { }
        Column& operator[](uint16_t colNum) { return data[colNum]; }
        const Column& operator[](uint16_t colNum) const { return data[colNum]; }
        void zeroFill() { for (Column &col : data) { col.zeroFill(); } }
    private:
        std::vector<Column> data;
    };

    void init(uint16_t layers, uint16_t sizeX, uint16_t sizeY);
    Layer& operator[](uint16_t layerNum) { return data[layerNum]; }
    const Layer& operator[](uint16_t layerNum) const { return data[layerNum]; }
    uint8_t getMagnitude(uint16_t layerNum, Coord loc) const { return (*this)[layerNum][loc.x][loc.y]; }
    //! Increases the specified location by centerIncreaseAmount,
    //! and increases the neighboring cells by neighborIncreaseAmount

    //! Is it ok that multiple readers are reading this container while
    //! this single thread is writing to it?  todo!!!
    void increment(uint16_t layerNum, Coord loc);
    void zeroFill() { for (Layer &layer : data) { layer.zeroFill(); } }
    //! Fades the signals.
    void fade(unsigned layerNum);
private:
    std::vector<Layer> data;

    const Parameters& m_Params;
};