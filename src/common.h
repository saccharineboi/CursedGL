// Copyright (C) 2022 saccharineboi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include <math.h>
#include <stdbool.h>

////////////////////////////////////////
/// NOTE: Unfortunately currently only the GCC compiler
/// is supported, but support for other compilers is
/// in the plans.
///
/// Some of them aren't used currently but reserved
/// for future use
////////////////////////////////////////
#ifdef __GNUC__
    #define TX_ALIGNED_BUFFER(TYPE, NAME, SIZE, ALIGNMENT) \
        TYPE NAME[SIZE] __attribute__((aligned(ALIGNMENT)))
    #define TX_FORCE_INLINE __attribute__((always_inline)) static inline
    #define TX_PACKED_STRUCT struct __attribute__((__packed__))
#else
    #define TX_ALIGNED_BUFFER(TYPE, NAME, SIZE, ALIGNMENT) \
        TYPE NAME[SIZE]
    #define TX_FORCE_INLINE static inline
    #define TX_PACKED_STRUCT struct
#endif

////////////////////////////////////////
/// Various mathematical constants that are
/// used by TRex's own functions. Of course
/// you are free to use them as well.
////////////////////////////////////////
#define TX_EPSILON  (1e-6f)
#define TX_PI       (3.1415927f)
#define TX_PI2      (TX_PI * 2.0f)
#define TX_PIH      (TX_PI * 0.5f)
#define TX_PIQ      (TX_PI * 0.25f)

////////////////////////////////////////
/// Returns true if a and b are approximately equal.
/// This function still cannot compare floats with
/// high accuracy but it's better than comparing
/// floats directly.
///
/// Note that the CMakeLists.txt file sets the
/// appropriate flag for the compiler to make
/// direct floating-point comparisons illegal. Check the
/// links below for more info:
///
/// 1. https://bitbashing.io/comparing-floats.html
/// 2. https://stackoverflow.com/questions/4548004/how-to-correctly-and-standardly-compare-floats
/// 3. https://pages.cs.wisc.edu/~david/courses/cs552/S12/handouts/goldberg-floating-point.pdf
////////////////////////////////////////
TX_FORCE_INLINE bool txFloatEquals(float a, float b)
{
    return fabsf(a - b) <= TX_EPSILON * fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b)));
}

////////////////////////////////////////
/// Returns true if a and b are within delta of each other.
/// This function is used for the rasterization of lines;
/// See txRasterizeLine in rasterizer.h for more info.
////////////////////////////////////////
TX_FORCE_INLINE bool txFloatWithin(float a, float b, float delta)
{
    return fabsf(a - b) <= delta;
}

////////////////////////////////////////
TX_FORCE_INLINE float txToRadians(float degrees)
{
    return degrees * (TX_PI / 180.0f);
}

////////////////////////////////////////
TX_FORCE_INLINE float txToDegrees(float radians)
{
    return radians * (180.0f / TX_PI);
}

////////////////////////////////////////
TX_FORCE_INLINE float txMax3(float f0, float f1, float f2)
{
    return fmaxf(f0, fmaxf(f1, f2));
}

////////////////////////////////////////
TX_FORCE_INLINE float txMin3(float f0, float f1, float f2)
{
    return fminf(f0, fminf(f1, f2));
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
