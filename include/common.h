// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include <math.h>
#include <stdbool.h>

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
#define TX_EPSILON  (1e-6f)
#define TX_PI       (3.1415927f)
#define TX_PI2      (TX_PI * 2.0f)
#define TX_PIH      (TX_PI * 0.5f)
#define TX_PIQ      (TX_PI * 0.25f)

////////////////////////////////////////
/// Note that the CMakeLists.txt file sets the
/// appropriate flag for the compiler to make
/// direct floating-point comparisons illegal. Check the
/// links below for more info:
///
/// * https://bitbashing.io/comparing-floats.html
/// * https://stackoverflow.com/questions/4548004/how-to-correctly-and-standardly-compare-floats
/// * https://pages.cs.wisc.edu/~david/courses/cs552/S12/handouts/goldberg-floating-point.pdf
////////////////////////////////////////
TX_FORCE_INLINE bool txFloatEquals(float a, float b)
{
    return fabsf(a - b) <= TX_EPSILON * fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b)));
}

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
