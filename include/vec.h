// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "common.h"

#include <math.h>
#include <string.h>

////////////////////////////////////////
/// Each TX_VEC{N}_SIZE is the size of N-vector
/// in bytes
////////////////////////////////////////
#define TX_VEC2_SIZE 2 * sizeof(float)
#define TX_VEC3_SIZE 3 * sizeof(float)
#define TX_VEC4_SIZE 4 * sizeof(float)

////////////////////////////////////////
/// Useful vector initializers
////////////////////////////////////////
#define TX_VEC2_ZERO { 0.0f, 0.0f }
#define TX_VEC3_ZERO { 0.0f, 0.0f, 0.0f }
#define TX_VEC4_ZERO { 0.0f, 0.0f, 0.0f, 0.0f }

#define TX_VEC2_ONE { 1.0f, 1.0f }
#define TX_VEC3_ONE { 1.0f, 1.0f, 1.0f }
#define TX_VEC4_ONE { 1.0f, 1.0f, 1.0f, 1.0f }
#define TX_VEC4_W1  { 0.0f, 0.0f, 0.0f, 1.0f }

#define TX_VEC3_XAXIS { 1.0f, 0.0f, 0.0f }
#define TX_VEC3_YAXIS { 0.0f, 1.0f, 0.0f }
#define TX_VEC3_ZAXIS { 0.0f, 0.0f, 1.0f }
////////////////////////////////////////

////////////////////////////////////////
#define TX_VEC_ZERO(N) \
    TX_FORCE_INLINE void txVec##N##Zero(TXvec##N v) \
    { \
        bzero(v, N * sizeof(float)); \
    }

////////////////////////////////////////
#define TX_VEC_COPY(N) \
    TX_FORCE_INLINE void txVec##N##Copy(TXvec##N dst, TXvec##N src) \
    { \
        if (dst == src) return; \
        memcpy(dst, src, N * sizeof(float)); \
    }

////////////////////////////////////////
#define TX_VEC_EQUALS(N) \
    TX_FORCE_INLINE bool txVec##N##Equals(TXvec##N a, TXvec##N b) \
    { \
        for (int i = 0; i < N; ++i) \
            if (!txFloatEquals(a[i], b[i])) \
                return false; \
        return true; \
    }

////////////////////////////////////////
/// Defines a binary arithmetic operation
/// between two N-dimensional vectors
////////////////////////////////////////
#define TX_VEC_BIN_OP(N, NAME, OP) \
    TX_FORCE_INLINE void txVec##N####NAME(TXvec##N r, TXvec##N a, TXvec##N b) \
    { \
        for (int i = 0; i < N; ++i) \
            r[i] = a[i] OP b[i]; \
    }

////////////////////////////////////////
/// Defines a binary arithmetic operation
/// between an N-dimensional vector and
/// a scalar value
////////////////////////////////////////
#define TX_VEC_BIN_SCALAR_OP(N, NAME, OP) \
    TX_FORCE_INLINE void txVec##N##Scalar##NAME(TXvec##N r, TXvec##N a, float s) \
    { \
        for (int i = 0; i < N; ++i) \
            r[i] = a[i] OP s; \
    }

////////////////////////////////////////
#define TX_VEC_NEGATE(N) \
    TX_FORCE_INLINE void txVec##N##Negate(TXvec##N r, TXvec##N v) \
    { \
        for (int i = 0; i < N; ++i) \
            r[i] = -v[i]; \
    }

////////////////////////////////////////
#define TX_VEC_DOT(N) \
    TX_FORCE_INLINE float txVec##N##Dot(TXvec##N a, TXvec##N b) \
    { \
        float res = 0.0f; \
        for (int i = 0; i < N; ++i) \
            res += a[i] * b[i]; \
        return res; \
    }

////////////////////////////////////////
#define TX_VEC_SQRLEN(N) \
    TX_FORCE_INLINE float txVec##N##Sqrlen(TXvec##N v) \
    { \
        float res = 0.0f; \
        for (int i = 0; i < N; ++i) \
            res += v[i] * v[i]; \
        return res; \
    }

////////////////////////////////////////
#define TX_VEC_LEN(N) \
    TX_FORCE_INLINE float txVec##N##Len(TXvec##N v) \
    { \
        float res = 0.0f; \
        for (int i = 0; i < N; ++i) \
            res += v[i] * v[i]; \
        return sqrtf(res); \
    }

////////////////////////////////////////
#define TX_VEC_NORMALIZE(N) \
    TX_FORCE_INLINE void txVec##N##Normalize(TXvec##N r, TXvec##N v) \
    { \
        float sum = 0.0f; \
        for (int i = 0; i < N; ++i) \
            sum += v[i] * v[i]; \
        if (!txFloatEquals(sum, 0.0f)) { \
            float factor = 1.0f / sqrtf(sum); \
            for (int i = 0; i < N; ++i) \
                r[i] = v[i] * factor; \
        } \
    }

////////////////////////////////////////
#define TX_VEC_LERP(N) \
    TX_FORCE_INLINE void txVec##N##Lerp(TXvec##N r, TXvec##N a, TXvec##N b, float s) \
    { \
        for (int i = 0; i < N; ++i) \
            r[i] = a[i] + s * (b[i] - a[i]); \
    }

////////////////////////////////////////
#define TX_VEC_DIST(N) \
    TX_FORCE_INLINE float txVec##N##Dist(TXvec##N a, TXvec##N b) \
    { \
        float sum = 0.0f; \
        for (int i = 0; i < N; ++i) { \
            float diff = b[i] - a[i]; \
            sum += diff * diff; \
        } \
        return sqrtf(sum); \
    }

////////////////////////////////////////
#define TX_VEC_SQRDIST(N) \
    TX_FORCE_INLINE float txVec##N##Sqrdist(TXvec##N a, TXvec##N b) \
    { \
        float sum = 0.0f; \
        for (int i = 0; i < N; ++i) { \
            float diff = b[i] - a[i]; \
            sum += diff * diff; \
        } \
        return sum; \
    }

////////////////////////////////////////
#define TX_VEC_INVERT(N) \
    TX_FORCE_INLINE void txVec##N##Invert(TXvec##N r, TXvec##N v) \
    { \
        for (int i = 0; i < N; ++i) \
            r[i] = 1.0f / v[i]; \
    }

////////////////////////////////////////
#define TX_VEC_CLAMP(N) \
    TX_FORCE_INLINE void txVec##N##Clamp(TXvec##N r, TXvec##N v, float min, float max) \
    { \
        for (int i = 0; i < N; ++i) { \
            if (v[i] > max) \
                r[i] = max; \
            else if (v[i] < min) \
                r[i] = min; \
            else \
                r[i] = v[i]; \
        } \
    }

////////////////////////////////////////
/// Macro to generate N-dimensional vectors
/// and corresponding functions that operate
/// on them
////////////////////////////////////////
#define TX_VEC_DEF(N) \
    typedef float TXvec##N[N]; \
    TX_VEC_ZERO(N) \
    TX_VEC_COPY(N) \
    TX_VEC_EQUALS(N) \
    TX_VEC_BIN_OP(N, Add, +) \
    TX_VEC_BIN_OP(N, Sub, -) \
    TX_VEC_BIN_OP(N, Mul, *) \
    TX_VEC_BIN_OP(N, Div, /) \
    TX_VEC_BIN_SCALAR_OP(N, Add, +) \
    TX_VEC_BIN_SCALAR_OP(N, Sub, -) \
    TX_VEC_BIN_SCALAR_OP(N, Mul, *) \
    TX_VEC_BIN_SCALAR_OP(N, Div, /) \
    TX_VEC_NEGATE(N) \
    TX_VEC_DOT(N) \
    TX_VEC_SQRLEN(N) \
    TX_VEC_LEN(N) \
    TX_VEC_NORMALIZE(N) \
    TX_VEC_LERP(N) \
    TX_VEC_DIST(N) \
    TX_VEC_SQRDIST(N) \
    TX_VEC_INVERT(N) \
    TX_VEC_CLAMP(N)

////////////////////////////////////////
TX_VEC_DEF(2)
TX_VEC_DEF(3)
TX_VEC_DEF(4)

////////////////////////////////////////
TX_FORCE_INLINE void txVec3Cross(TXvec3 r, TXvec3 a, TXvec3 b)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec4DivideByW(TXvec4 r, TXvec4 v)
{
    r[0] = v[0] / v[3];
    r[1] = v[1] / v[3];
    r[2] = v[2] / v[3];
    r[3] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec2Set(TXvec2 v, float x, float y)
{
    v[0] = x;
    v[1] = y;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec3Set(TXvec3 v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec4Set(TXvec4 v, float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec3RotateX(TXvec3 res, TXvec3 vec, float rads)
{
    float s = sinf(rads);
    float c = cosf(rads);

    float v1 = vec[1], v2 = vec[2];

    res[0] = vec[0];
    res[1] = v1 * c - v2 * s;
    res[2] = v1 * s + v2 * c;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec3RotateY(TXvec3 res, TXvec3 vec, float rads)
{
    float s = sinf(rads);
    float c = cosf(rads);

    float v0 = vec[0], v2 = vec[2];

    res[0] = v2 * s + v0 * c;
    res[1] = vec[1];
    res[2] = v2 * c - v0 * s;
}

////////////////////////////////////////
TX_FORCE_INLINE void txVec3RotateZ(TXvec3 res, TXvec3 vec, float rads)
{
    float s = sinf(rads);
    float c = cosf(rads);

    float v0 = vec[0], v1 = vec[1];

    res[0] = v0 * c - v1 * s;
    res[1] = v0 * s + v1 * c;
    res[2] = vec[2];
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
