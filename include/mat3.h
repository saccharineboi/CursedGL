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
typedef float TXmat3[9];

////////////////////////////////////////
#define TX_MAT3_IDENTITY { 1.0f, 0.0f, 0.0f, \
                           0.0f, 1.0f, 0.0f, \
                           0.0f, 0.0f, 1.0f }

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Zero(TXmat3 m)
{
    bzero(m, 9 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Identity(TXmat3 m)
{
    for (int i = 0; i < 9; ++i)
        m[i] = (!i || !(i % 5)) ? 1.0f : 0.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Copy(TXmat3 dst, TXmat3 src)
{
    memcpy(dst, src, 9 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE bool txMat3Equals(TXmat3 a, TXmat3 b)
{
    for (int i = 0; i < 9; ++i)
        if (!txFloatEquals(a[i], b[i]))
            return false;
    return true;
}

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for two given matrices
////////////////////////////////////////
#define TX_GEN_MAT3_BIN_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat3##NAME(TXmat3 r, TXmat3 a, TXmat3 b) \
    { \
        for (int i = 0; i < 9; ++i) \
            r[i] = a[i] OP b[i]; \
    }

////////////////////////////////////////
TX_GEN_MAT3_BIN_OP(Add, +)
TX_GEN_MAT3_BIN_OP(Sub, -)
TX_GEN_MAT3_BIN_OP(Div, /)

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for a given matrix and scalar
////////////////////////////////////////
#define TX_GEN_MAT3_BIN_SCALAR_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat3##NAME##Scalar(TXmat3 r, TXmat3 a, float s) \
    { \
        for (int i = 0; i < 9; ++i) \
            r[i] = a[i] OP s; \
    }

////////////////////////////////////////
TX_GEN_MAT3_BIN_SCALAR_OP(Add, +)
TX_GEN_MAT3_BIN_SCALAR_OP(Sub, -)
TX_GEN_MAT3_BIN_SCALAR_OP(Mul, *)
TX_GEN_MAT3_BIN_SCALAR_OP(Div, /)

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Mul(TXmat3 r, TXmat3 a, TXmat3 b)
{
    float a0 = a[0], a3 = a[3], a6 = a[6];
    float a1 = a[1], a4 = a[4], a7 = a[7];
    float a2 = a[2], a5 = a[5], a8 = a[8];

    float b0 = b[0],
          b1 = b[1],
          b2 = b[2];

    r[0] = b0 * a0 + b1 * a3 + b2 * a6;
    r[1] = b0 * a1 + b1 * a4 + b2 * a7;
    r[2] = b0 * a2 + b1 * a5 + b2 * a8;

    b0 = b[3];
    b1 = b[4];
    b2 = b[5];

    r[3] = b0 * a0 + b1 * a3 + b2 * a6;
    r[4] = b0 * a1 + b1 * a4 + b2 * a7;
    r[5] = b0 * a2 + b1 * a5 + b2 * a8;

    b0 = b[6];
    b1 = b[7];
    b2 = b[8];

    r[6] = b0 * a0 + b1 * a3 + b2 * a6;
    r[7] = b0 * a1 + b1 * a4 + b2 * a7;
    r[8] = b0 * a2 + b1 * a5 + b2 * a8;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Negate(TXmat3 r, TXmat3 m)
{
    for (int i = 0; i < 9; ++i)
        r[i] = -m[i];
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Transpose(TXmat3 r, TXmat3 m)
{
    float m0 = m[0], m3 = m[3], m6 = m[6];
    float m1 = m[1], m4 = m[4], m7 = m[7];
    float m2 = m[2], m5 = m[5], m8 = m[8];

    r[0] = m0;
    r[1] = m3;
    r[2] = m6;

    r[3] = m1;
    r[4] = m4;
    r[5] = m7;

    r[6] = m2;
    r[7] = m5;
    r[8] = m8;
}

////////////////////////////////////////
TX_FORCE_INLINE float txMat3Det(TXmat3 m)
{
    float m0 = m[0], m3 = m[3], m6 = m[6];
    float m1 = m[1], m4 = m[4], m7 = m[7];
    float m2 = m[2], m5 = m[5], m8 = m[8];

    return m0 * ( m8 * m4 - m5 * m7) +
           m1 * (-m8 * m3 + m5 * m6) +
           m2 * ( m7 * m3 - m4 * m6);
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat3Invert(TXmat3 r, TXmat3 m)
{
    float m0 = m[0], m3 = m[3], m6 = m[6];
    float m1 = m[1], m4 = m[4], m7 = m[7];
    float m2 = m[2], m5 = m[5], m8 = m[8];

    float k0 =  m8 * m4 - m5 * m7;
    float k1 = -m8 * m3 + m5 * m6;
    float k2 =  m7 * m3 - m4 * m6;

    float det = m0 * k0 + m1 * k1 + m2 * k2;
    if (!txFloatEquals(det, 0.0f)) {
        det = 1.0f / det;

        r[0] = k0 * det;
        r[1] = (-m8 * m1 + m2 * m7) * det;
        r[2] = (m5 * m1 - m2 * m4) * det;

        r[3] = k1 * det;
        r[4] = (m8 * m0 - m2 * m6) * det;
        r[5] = (-m5 * m0 + m2 * m3) * det;

        r[6] = k2 * det;
        r[7] = (-m7 * m0 + m1 * m6) * det;
        r[8] = (m4 * m0 - m1 * m3) * det;
    }
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
