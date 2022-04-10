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

// NOTE: matrices are stored in memory
// in column-major order

////////////////////////////////////////
typedef float TXmat2[4];

////////////////////////////////////////
#define TX_MAT2_IDENTITY { 1.0f, 0.0f, \
                           0.0f, 1.0f }

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Zero(TXmat2 m)
{
    bzero(m, 4 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Identity(TXmat2 m)
{
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Copy(TXmat2 dst, TXmat2 src)
{
    memcpy(dst, src, 4 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE bool txMat2Equals(TXmat2 a, TXmat2 b)
{
    for (int i = 0; i < 4; ++i)
        if (!txFloatEquals(a[i], b[i]))
            return false;
    return true;
}

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for two given matrices
////////////////////////////////////////
#define TX_GEN_MAT2_BIN_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat2##NAME(TXmat2 r, TXmat2 a, TXmat2 b) \
    { \
        for (int i = 0; i < 4; ++i) \
            r[i] = a[i] OP b[i]; \
    }

////////////////////////////////////////
TX_GEN_MAT2_BIN_OP(Add, +)
TX_GEN_MAT2_BIN_OP(Sub, -)
TX_GEN_MAT2_BIN_OP(Div, /)

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for a given matrix and scalar
////////////////////////////////////////
#define TX_GEN_MAT2_BIN_SCALAR_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat2##NAME##Scalar(TXmat2 r, TXmat2 a, float s) \
    { \
        for (int i = 0; i < 4; ++i) \
            r[i] = a[i] OP s; \
    }

////////////////////////////////////////
TX_GEN_MAT2_BIN_SCALAR_OP(Add, +)
TX_GEN_MAT2_BIN_SCALAR_OP(Sub, -)
TX_GEN_MAT2_BIN_SCALAR_OP(Mul, *)
TX_GEN_MAT2_BIN_SCALAR_OP(Div, /)

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Mul(TXmat2 r, TXmat2 a, TXmat2 b)
{
    float a0 = a[0], a2 = a[2];
    float a1 = a[1], a3 = a[3];

    float b0 = b[0], b2 = b[2];
    float b1 = b[1], b3 = b[3];

    r[0] = a0 * b0 + a2 * b1;
    r[1] = a1 * b0 + a3 * b1;
    r[2] = a0 * b2 + a2 * b3;
    r[3] = a1 * b2 + a3 * b3;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Negate(TXmat2 r, TXmat2 m)
{
    for (int i = 0; i < 4; ++i)
        r[i] = -m[i];
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Transpose(TXmat2 r, TXmat2 m)
{
    r[0] = m[0];
    float tmp = m[1];
    r[1] = m[2];
    r[2] = tmp;
    r[3] = m[3];
}

////////////////////////////////////////
TX_FORCE_INLINE float txMat2Det(TXmat2 m)
{
    return m[0] * m[3] - m[1] * m[2];
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat2Invert(TXmat2 r, TXmat2 m)
{
    float det = m[0] * m[3] - m[1] * m[2];
    if (!txFloatEquals(det, 0.0f)) {
        det = 1.0f / det;
        r[0] = det *  m[3];
        r[1] = det * -m[1];
        r[2] = det * -m[2];
        r[3] = det *  m[0];
    }
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
