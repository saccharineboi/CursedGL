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
#include <stdio.h>

// NOTE: matrices are stored in memory
// in column-major order

////////////////////////////////////////
typedef float TXmat4[16];

////////////////////////////////////////
#define TX_MAT4_IDENTITY { 1.0f, 0.0f, 0.0f, 0.0f, \
                           0.0f, 1.0f, 0.0f, 0.0f, \
                           0.0f, 0.0f, 1.0f, 0.0f, \
                           0.0f, 0.0f, 0.0f, 1.0f }

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Zero(TXmat4 m)
{
    bzero(m, 16 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Identity(TXmat4 m)
{
    for (int i = 0; i < 16; ++i)
        m[i] = (!i || !(i % 5)) ? 1.0f : 0.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Copy(TXmat4 dst, TXmat4 src)
{
    memcpy(dst, src, 16 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE bool txMat4Equals(TXmat4 a, TXmat4 b)
{
    for (int i = 0; i < 16; ++i)
        if (!txFloatEquals(a[i], b[i]))
            return false;
    return true;
}

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for two given matrices
////////////////////////////////////////
#define TX_GEN_MAT4_BIN_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat4##NAME(TXmat4 r, TXmat4 a, TXmat4 b) \
    { \
        for (int i = 0; i < 16; ++i) \
            r[i] = a[i] OP b[i]; \
    }

////////////////////////////////////////
TX_GEN_MAT4_BIN_OP(Add, +)
TX_GEN_MAT4_BIN_OP(Sub, -)
TX_GEN_MAT4_BIN_OP(Div, /)

////////////////////////////////////////
/// Generates binary arithmetic operators
/// for a given matrix and scalar
////////////////////////////////////////
#define TX_GEN_MAT4_BIN_SCALAR_OP(NAME, OP) \
    TX_FORCE_INLINE void txMat4##NAME##Scalar(TXmat4 r, TXmat4 a, float s) \
    { \
        for (int i = 0; i < 16; ++i) \
            r[i] = a[i] OP s; \
    }

////////////////////////////////////////
TX_GEN_MAT4_BIN_SCALAR_OP(Add, +)
TX_GEN_MAT4_BIN_SCALAR_OP(Sub, -)
TX_GEN_MAT4_BIN_SCALAR_OP(Mul, *)
TX_GEN_MAT4_BIN_SCALAR_OP(Div, /)

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Mul(TXmat4 r, TXmat4 a, TXmat4 b)
{
    float a0 = a[0], a4 = a[4], a8  = a[8],  a12 = a[12];
    float a1 = a[1], a5 = a[5], a9  = a[9],  a13 = a[13];
    float a2 = a[2], a6 = a[6], a10 = a[10], a14 = a[14];
    float a3 = a[3], a7 = a[7], a11 = a[11], a15 = a[15];

    float b0 = b[0],
          b1 = b[1],
          b2 = b[2],
          b3 = b[3];

    r[0] = b0 * a0 + b1 * a4 + b2 * a8  + b3 * a12;
    r[1] = b0 * a1 + b1 * a5 + b2 * a9  + b3 * a13;
    r[2] = b0 * a2 + b1 * a6 + b2 * a10 + b3 * a14;
    r[3] = b0 * a3 + b1 * a7 + b2 * a11 + b3 * a15;

    b0 = b[4];
    b1 = b[5];
    b2 = b[6];
    b3 = b[7];

    r[4] = b0 * a0 + b1 * a4 + b2 * a8  + b3 * a12;
    r[5] = b0 * a1 + b1 * a5 + b2 * a9  + b3 * a13;
    r[6] = b0 * a2 + b1 * a6 + b2 * a10 + b3 * a14;
    r[7] = b0 * a3 + b1 * a7 + b2 * a11 + b3 * a15;

    b0 = b[8];
    b1 = b[9];
    b2 = b[10];
    b3 = b[11];

    r[8]  = b0 * a0 + b1 * a4 + b2 * a8  + b3 * a12;
    r[9]  = b0 * a1 + b1 * a5 + b2 * a9  + b3 * a13;
    r[10] = b0 * a2 + b1 * a6 + b2 * a10 + b3 * a14;
    r[11] = b0 * a3 + b1 * a7 + b2 * a11 + b3 * a15;

    b0 = b[12];
    b1 = b[13];
    b2 = b[14];
    b3 = b[15];

    r[12] = b0 * a0 + b1 * a4 + b2 * a8  + b3 * a12;
    r[13] = b0 * a1 + b1 * a5 + b2 * a9  + b3 * a13;
    r[14] = b0 * a2 + b1 * a6 + b2 * a10 + b3 * a14;
    r[15] = b0 * a3 + b1 * a7 + b2 * a11 + b3 * a15;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Negate(TXmat4 r, TXmat4 m)
{
    for (int i = 0; i < 16; ++i)
        r[i] = -m[i];
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Transpose(TXmat4 r, TXmat4 m)
{
    float m0 = m[0], m4 = m[4], m8  = m[8],  m12 = m[12];
    float m1 = m[1], m5 = m[5], m9  = m[9],  m13 = m[13];
    float m2 = m[2], m6 = m[6], m10 = m[10], m14 = m[14];
    float m3 = m[3], m7 = m[7], m11 = m[11], m15 = m[15];

    r[0]  = m0;
    r[1]  = m4;
    r[2]  = m8;
    r[3]  = m12;
    r[4]  = m1;
    r[5]  = m5;
    r[6]  = m9;
    r[7]  = m13;
    r[8]  = m2;
    r[9]  = m6;
    r[10] = m10;
    r[11] = m14;
    r[12] = m3;
    r[13] = m7;
    r[14] = m11;
    r[15] = m15;
}

////////////////////////////////////////
TX_FORCE_INLINE float txMat4Det(TXmat4 m)
{
    float b0 = m[0]  * m[5]  - m[1]  * m[4];
    float b1 = m[0]  * m[6]  - m[2]  * m[4];
    float b2 = m[1]  * m[6]  - m[2]  * m[5];
    float b3 = m[8]  * m[13] - m[9]  * m[12];
    float b4 = m[8]  * m[14] - m[10] * m[12];
    float b5 = m[9]  * m[14] - m[10] * m[13];
    float b6 = m[0]  * b5    - m[1]  * b4 + m[2]  * b3;
    float b7 = m[4]  * b5    - m[5]  * b4 + m[6]  * b3;
    float b8 = m[8]  * b2    - m[9]  * b1 + m[10] * b0;
    float b9 = m[12] * b2    - m[13] * b1 + m[14] * b0;

    return m[7] * b6 - m[3] * b7 + m[15] * b8 - m[11] * b9;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMat4Invert(TXmat4 r, TXmat4 m)
{
    float m00 = m[0],
          m01 = m[1],
          m02 = m[2],
          m03 = m[3];

    float m10 = m[4],
          m11 = m[5],
          m12 = m[6],
          m13 = m[7];

    float m20 = m[8],
          m21 = m[9],
          m22 = m[10],
          m23 = m[11];

    float m30 = m[12],
          m31 = m[13],
          m32 = m[14],
          m33 = m[15];

    float b00 = m00 * m11 - m01 * m10;
    float b01 = m00 * m12 - m02 * m10;
    float b02 = m00 * m13 - m03 * m10;
    float b03 = m01 * m12 - m02 * m11;
    float b04 = m01 * m13 - m03 * m11;
    float b05 = m02 * m13 - m03 * m12;
    float b06 = m20 * m31 - m21 * m30;
    float b07 = m20 * m32 - m22 * m30;
    float b08 = m20 * m33 - m23 * m30;
    float b09 = m21 * m32 - m22 * m31;
    float b10 = m21 * m33 - m23 * m31;
    float b11 = m22 * m33 - m23 * m32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (txFloatEquals(det, 0.0f))
        return;

    det = 1.0f / det;
    r[0]  = (m11 * b11 - m12 * b10 + m13 * b09) * det;
    r[1]  = (m02 * b10 - m01 * b11 - m03 * b09) * det;
    r[2]  = (m31 * b05 - m32 * b04 + m33 * b03) * det;
    r[3]  = (m22 * b04 - m21 * b05 - m23 * b03) * det;
    r[4]  = (m12 * b08 - m10 * b11 - m13 * b07) * det;
    r[5]  = (m00 * b11 - m02 * b08 + m03 * b07) * det;
    r[6]  = (m32 * b02 - m30 * b05 - m33 * b01) * det;
    r[7]  = (m20 * b05 - m22 * b02 + m23 * b01) * det;
    r[8]  = (m10 * b10 - m11 * b08 + m13 * b06) * det;
    r[9]  = (m01 * b08 - m00 * b10 - m03 * b06) * det;
    r[10] = (m30 * b04 - m31 * b02 + m33 * b00) * det;
    r[11] = (m21 * b02 - m20 * b04 - m23 * b00) * det;
    r[12] = (m11 * b07 - m10 * b09 - m12 * b06) * det;
    r[13] = (m00 * b09 - m01 * b07 + m02 * b06) * det;
    r[14] = (m31 * b01 - m30 * b03 - m32 * b00) * det;
    r[15] = (m20 * b03 - m21 * b01 + m22 * b00) * det;
}


////////////////////////////////////////
TX_FORCE_INLINE void txMat4Print(TXmat4 m)
{
    printf("mat4:\t[%.2f, %.2f, %.2f, %.2f]\n", m[0],  m[1],  m[2],  m[3]);
    printf("\t[%.2f, %.2f, %.2f, %.2f]\n",      m[4],  m[5],  m[6],  m[7]);
    printf("\t[%.2f, %.2f, %.2f, %.2f]\n",      m[8],  m[9],  m[10], m[11]);
    printf("\t[%.2f, %.2f, %.2f, %.2f]\n",      m[12], m[13], m[14], m[15]);
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
