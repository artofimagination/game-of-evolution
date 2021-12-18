#pragma once

#include "Random.h"

#include <cmath>
#include <cstdint>

/*
Basic types used throughout the project:

Compass - an enum with enumerants SW, S, SE, W, CENTER, E, NW, N, NE

    Compass arithmetic values:

        6  7  8
        3  4  5
        0  1  2

Dir, Coord, Polar, and their constructors:

    Dir - abstract type for 8 directions plus center
    ctor Dir(Compass = CENTER)

    Coord - signed int16_t pair, absolute location or difference of locations
    ctor Coord() = 0,0

    Polar - signed magnitude and direction
    ctor Polar(Coord = 0,0)

Conversions

    uint8_t = Dir.asInt()

    Dir = Coord.asDir()
    Dir = Polar.asDir()

    Coord = Dir.asNormalizedCoord()
    Coord = Polar.asCoord()

    Polar = Dir.asNormalizedPolar()
    Polar = Coord.asPolar()

Arithmetic

    Dir.rotate(int n = 0)

    Coord = Coord + Dir
    Coord = Coord + Coord
    Coord = Coord + Polar

    Polar = Polar + Coord (additive)
    Polar = Polar + Polar (additive)
    Polar = Polar * Polar (dot product)
*/

//! Defines compass directions
enum class Compass : uint8_t 
{ 
    SW = 0,
    S,
    SE,
    W,
    CENTER,
    E,
    NW,
    N,
    NE
};

class Coord;
class Polar;

//! Supports the eight directions in enum class Compass plus CENTER.
struct __attribute__((packed)) Dir 
{
    static Dir random8(RandomUintGenerator& r) { return Dir(Compass::N).rotate(r(0, 7)); }

    Dir(Compass dir = Compass::CENTER) : dir9{dir} {}
    Dir& operator=(const Compass& d) { dir9 = d; return *this; }
    uint8_t asInt() const { return (uint8_t)dir9; }
    /*!
    A normalized Coord is a Coord with x and y == -1, 0, or 1.
    A normalized Coord may be used as an offset to one of the
    8-neighbors.

    A Dir value maps to a normalized Coord using

       Coord { (d%3) - 1, (trunc)(d/3) - 1  }

       0 => -1, -1
       1 =>  0, -1
       2 =>  1, -1,
       3 => -1,  0
       4 =>  0,  0
       5 =>  1   0
       6 => -1,  1
       7 =>  0,  1
       8 =>  1,  1
    */
    Coord asNormalizedCoord() const;  // (-1, -0, 1, -1, 0, 1)
    Polar asNormalizedPolar() const;

    //! This rotates a Dir value by the specified number of steps. There are
    //! eight steps per full rotation. Positive values are clockwise; negative
    //! values are counterclockwise. E.g., rotate(4) returns a direction 90
    //! degrees to the right.
    Dir rotate(int n = 0) const;
    Dir rotate90DegCW() const { return rotate(2); }
    Dir rotate90DegCCW() const { return rotate(-2); }
    Dir rotate180Deg() const { return rotate(4); }

    bool operator==(Compass d) const { return asInt() == (uint8_t)d; }
    bool operator!=(Compass d) const { return asInt() != (uint8_t)d; }
    bool operator==(Dir d) const { return asInt() == d.asInt(); }
    bool operator!=(Dir d) const { return asInt() != d.asInt(); }
private:
    Compass dir9;
};


//! Coordinates range anywhere in the range of int16_t. Coordinate arithmetic
//! wraps like int16_t. Can be used, e.g., for a location in the simulator grid, or
//! for the difference between two locations.
struct __attribute__((packed)) Coord
{
    Coord(int16_t x0 = 0, int16_t y0 = 0) : x{x0}, y{y0} { }
    bool isNormalized() const { return x >= -1 && x <= 1 && y >= -1 && y <= 1; }
    /*!
        A normalized Coord has x and y == -1, 0, or 1.
        A normalized Coord may be used as an offset to one of the
        8-neighbors.
        We'll convert the Coord into a Dir, then convert Dir to normalized Coord.
    */
    Coord normalize() const;
    unsigned length() const { return (int)(std::sqrt(x * x + y * y)); } // round down
    //! Effectively, we want to check if a coordinate lies in a 45 degree region (22.5 degrees each side)
    //! centered on each compass direction. By first rotating the system by 22.5 degrees clockwise
    //! the boundaries to these regions become much easier to work with as they just align with the 8 axes.
    //! (Thanks to @Asa-Hopkins for this optimization -- drm)
    Dir asDir() const;
    Polar asPolar() const;

    bool operator==(Coord c) const { return x == c.x && y == c.y; }
    bool operator!=(Coord c) const { return x != c.x || y != c.y; }
    Coord operator+(Coord c) const { return Coord{(int16_t)(x + c.x), (int16_t)(y + c.y)}; }
    Coord operator-(Coord c) const { return Coord{(int16_t)(x - c.x), (int16_t)(y - c.y)}; }
    Coord operator*(int a) const { return Coord{(int16_t)(x * a), (int16_t)(y * a)}; }
    Coord operator+(Dir d) const { return *this + d.asNormalizedCoord(); }
    Coord operator-(Dir d) const { return *this - d.asNormalizedCoord(); }

    //! returns -1.0 (opposite directions) .. +1.0 (same direction)
    //! returns 1.0 if either vector is (0,0)
    float raySameness(Coord other) const;
    //! returns -1.0 (opposite directions) .. +1.0 (same direction)
    //! returns 1.0 if self is (0,0) or d is CENTER
    float raySameness(Dir d) const;
public:
    int16_t x;
    int16_t y;
};


//! Polar magnitudes are signed 32-bit integers so that they can extend across any 2D
//! area defined by the Coord class.
struct __attribute__((packed)) Polar 
{
    explicit Polar(int mag0 = 0, Compass dir0 = Compass::CENTER)
         : mag{mag0}, dir{Dir{dir0}} { }
    explicit Polar(int mag0, Dir dir0)
         : mag{mag0}, dir{dir0} { }
    /*!
    Compass values:

        6  7  8
        3  4  5
        0  1  2
    */
    Coord asCoord() const;
public:
    int mag;
    Dir dir;
};