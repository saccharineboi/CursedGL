// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "common.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// NOTE: Given a quaternion q = ai + bj + ck + d
// where i * j * k = -1 is stored as
// [ d, a, b, c ] in memory

////////////////////////////////////////
typedef float TXquat[4];

////////////////////////////////////////
#define TX_QUAT_IDENTITY { 1.0f, 0.0f, 0.0f, 0.0f }

////////////////////////////////////////
TX_FORCE_INLINE void txQuatZero(TXquat q)
{
    bzero(q, 4 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatInit(TXquat q)
{
    bzero(&q[1], 3 * sizeof(float));
    q[0] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatCreate(TXquat q, float rads, float x, float y, float z)
{
    float s_rads = sinf(rads * 0.5f);
    float c_rads = cosf(rads * 0.5f);

    q[0] = c_rads;
    q[1] = s_rads * x;
    q[2] = s_rads * y;
    q[3] = s_rads * z;
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatCopy(TXquat dst, TXquat src)
{
    memcpy(dst, src, 4 * sizeof(float));
}

////////////////////////////////////////
TX_FORCE_INLINE bool txQuatEquals(TXquat a, TXquat b)
{
    for (int i = 0; i < 4; ++i)
        if (!txFloatEquals(a[i], b[i]))
            return false;
    return true;
}

////////////////////////////////////////
/// Generates arithmetic binary operators
/// between two quaternions
////////////////////////////////////////
#define TX_GEN_QUAT_BIN_OP(NAME, OP) \
    TX_FORCE_INLINE void txQuat##NAME (TXquat q, TXquat a, TXquat b) \
    { \
        for (int i = 0; i < 4; ++i) \
            q[i] = a[i] OP b[i]; \
    }

////////////////////////////////////////
TX_GEN_QUAT_BIN_OP(Add, +)
TX_GEN_QUAT_BIN_OP(Sub, -)
TX_GEN_QUAT_BIN_OP(Div, /)

////////////////////////////////////////
TX_FORCE_INLINE void txQuatMul(TXquat q, TXquat a, TXquat b)
{
    q[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3];
    q[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] + a[3] * b[2];
    q[2] = a[0] * b[2] + a[2] * b[0] + a[1] * b[3] + a[3] * b[1];
    q[3] = a[0] * b[3] + a[3] * b[0] + a[1] * b[2] + a[2] * b[1];
}

////////////////////////////////////////
/// Generates arithmetic binary operators
/// between a quaternion and a scalar
////////////////////////////////////////
#define TX_GEN_QUAT_BIN_SCALAR_OP(NAME, OP) \
    TX_FORCE_INLINE void txQuat##NAME##Scalar (TXquat q, TXquat a, float s) \
    { \
        for (int i = 0; i < 4; ++i) \
            q[i] = a[i] OP s; \
    }

////////////////////////////////////////
TX_GEN_QUAT_BIN_SCALAR_OP(Add, +)
TX_GEN_QUAT_BIN_SCALAR_OP(Sub, -)
TX_GEN_QUAT_BIN_SCALAR_OP(Mul, *)
TX_GEN_QUAT_BIN_SCALAR_OP(Div, /)

////////////////////////////////////////
TX_FORCE_INLINE void txQuatConjugate(TXquat q, TXquat a)
{
    q[0] =  a[0];
    q[1] = -a[1];
    q[2] = -a[2];
    q[3] = -a[3];
}

////////////////////////////////////////
TX_FORCE_INLINE float txQuatLen(TXquat q)
{
    return sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

////////////////////////////////////////
TX_FORCE_INLINE float txQuatSqrlen(TXquat q)
{
    return q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatNormalize(TXquat q, TXquat a)
{
    float sum = txQuatSqrlen(a);
    if (!txFloatEquals(sum, 0.0f)) {
        float multiplier = 1.0f / sqrtf(sum);
        q[0] = a[0] * multiplier;
        q[1] = a[1] * multiplier;
        q[2] = a[2] * multiplier;
        q[3] = a[3] * multiplier;
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatInverse(TXquat q, TXquat a)
{
    float sum = txQuatSqrlen(a);
    if (!txFloatEquals(sum, 0.0f)) {
        float multiplier = 1.0f / sqrtf(sum);
        q[0] =  multiplier * a[0];
        q[1] = -multiplier * a[1];
        q[2] = -multiplier * a[2];
        q[3] = -multiplier * a[3];
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txQuatPrint(TXquat q)
{
    printf("quat: [%.2f, %.2f, %.2f, %.2f]\n", q[3], q[0], q[1], q[2]);
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
