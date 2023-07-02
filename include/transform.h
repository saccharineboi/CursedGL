// Copyright (C) 2023 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "vec.h"
#include "mat4.h"
#include "quat.h"

////////////////////////////////////////
TX_FORCE_INLINE void txGenTranslationMat4(TXmat4 m, TXvec3 t)
{
    m[0]  = 1.0f;
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;

    m[4]  = 0.0f;
    m[5]  = 1.0f;
    m[6]  = 0.0f;
    m[7]  = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;

    m[12] = t[0];
    m[13] = t[1];
    m[14] = t[2];
    m[15] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txTranslateMat4(TXmat4 m, TXvec3 t)
{
    m[12] = m[0] * t[0] + m[4] * t[1] + m[8]  * t[2] + m[12];
    m[13] = m[1] * t[0] + m[5] * t[1] + m[9]  * t[2] + m[13];
    m[14] = m[2] * t[0] + m[6] * t[1] + m[10] * t[2] + m[14];
    m[15] = m[3] * t[0] + m[7] * t[1] + m[11] * t[2] + m[15];
}

////////////////////////////////////////
TX_FORCE_INLINE void txGenScaleMat4(TXmat4 m, TXvec3 s)
{
    m[0]  = s[0];
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;

    m[4]  = 0.0f;
    m[5]  = s[1];
    m[6]  = 0.0f;
    m[7]  = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = s[2];
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txScaleMat4(TXmat4 m, TXvec3 s)
{
    m[0]  *= s[0];
    m[1]  *= s[0];
    m[2]  *= s[0];
    m[3]  *= s[0];

    m[4]  *= s[1];
    m[5]  *= s[1];
    m[6]  *= s[1];
    m[7]  *= s[1];

    m[8]  *= s[2];
    m[9]  *= s[2];
    m[10] *= s[2];
    m[11] *= s[2];
}

////////////////////////////////////////
TX_FORCE_INLINE void txGenRotationMat4(TXmat4 m, TXquat q)
{
    float s = q[0];
    float x = q[1];
    float y = q[2];
    float z = q[3];

    float xx = 2.0f * x * x;
    float yy = 2.0f * y * y;
    float zz = 2.0f * z * z;

    float xy = 2.0f * x * y;
    float xz = 2.0f * x * z;
    float yz = 2.0f * y * z;

    float sz = 2.0f * s * z;
    float sy = 2.0f * s * y;
    float sx = 2.0f * s * x;

    m[0]  = 1.0f - yy - zz;
    m[1]  = xy + sz;
    m[2]  = xz - sy;
    m[3]  = 0.0f;

    m[4]  = xy - sz;
    m[5]  = 1.0f - xx - zz;
    m[6]  = yz + sx;
    m[7]  = 0.0f;

    m[8]  = xz + sy;
    m[9]  = yz - sx;
    m[10] = 1.0f - xx - yy;
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
}

////////////////////////////////////////
TX_FORCE_INLINE void txRotateMat4(TXmat4 m, float rads, float xAxis,
                                                        float yAxis,
                                                        float zAxis)
{
    float len = xAxis * xAxis + yAxis * yAxis + zAxis * zAxis;
    if (txFloatEquals(len, 0.0f))
        return;
    len = 1.0f / sqrtf(len);

    xAxis *= len;
    yAxis *= len;
    zAxis *= len;

    float s = sinf(rads);
    float c = cosf(rads);
    float t = 1.0f - c;

    float r00 = xAxis * xAxis * t + c;
    float r01 = yAxis * xAxis * t + zAxis * s;
    float r02 = zAxis * xAxis * t - yAxis * s;

    float r10 = xAxis * yAxis * t - zAxis * s;
    float r11 = yAxis * yAxis * t + c;
    float r12 = zAxis * yAxis * t + xAxis * s;

    float r20 = xAxis * zAxis * t + yAxis * s;
    float r21 = yAxis * zAxis * t - xAxis * s;
    float r22 = zAxis * zAxis * t + c;

    float m00 = m[0],  m01 = m[1],  m02 = m[2],  m03 = m[3],
          m10 = m[4],  m11 = m[5],  m12 = m[6],  m13 = m[7],
          m20 = m[8],  m21 = m[9],  m22 = m[10], m23 = m[11];

    m[0]  = m00 * r00 + m10 * r01 + m20 * r02;
    m[1]  = m01 * r00 + m11 * r01 + m21 * r02;
    m[2]  = m02 * r00 + m12 * r01 + m22 * r02;
    m[3]  = m03 * r00 + m13 * r01 + m23 * r02;
    m[4]  = m00 * r10 + m10 * r11 + m20 * r12;
    m[5]  = m01 * r10 + m11 * r11 + m21 * r12;
    m[6]  = m02 * r10 + m12 * r11 + m22 * r12;
    m[7]  = m03 * r10 + m13 * r11 + m23 * r12;
    m[8]  = m00 * r20 + m10 * r21 + m20 * r22;
    m[9]  = m01 * r20 + m11 * r21 + m21 * r22;
    m[10] = m02 * r20 + m12 * r21 + m22 * r22;
    m[11] = m03 * r20 + m13 * r21 + m23 * r22;
}

////////////////////////////////////////
TX_FORCE_INLINE void txGenModelMat4(TXmat4 m, TXvec3 t, TXquat q, TXvec3 s)
{
    float scalar = q[0];
    float x = q[1];
    float y = q[2];
    float z = q[3];

    float xx = 2.0f * x * x;
    float yy = 2.0f * y * y;
    float zz = 2.0f * z * z;

    float xy = 2.0f * x * y;
    float xz = 2.0f * x * z;
    float yz = 2.0f * y * z;

    float sz = 2.0f * scalar * z;
    float sy = 2.0f * scalar * y;
    float sx = 2.0f * scalar * x;

    float a = 1.0f - yy - zz;
    float b = xy + sz;
    float c = xz - sy;

    float d = xy - sz;
    float e = 1.0f - xx - zz;
    float f = yz + sx;

    float g = xz + sy;
    float h = yz - sx;
    float i = 1.0f - xx - yy;

    m[0]  = a * s[0];
    m[1]  = b * s[0];
    m[2]  = c * s[0];
    m[3]  = 0.0f;

    m[4]  = d * s[1];
    m[5]  = e * s[1];
    m[6]  = f * s[1];
    m[7]  = 0.0f;

    m[8]  = g * s[2];
    m[9]  = h * s[2];
    m[10] = i * s[2];
    m[11] = 0.0f;

    m[12] = t[0];
    m[13] = t[1];
    m[14] = t[2];
    m[15] = 1.0f;
}

////////////////////////////////////////
void txGenPerspectiveProjectionMatrix(TXmat4 m,
                                      float fovy,
                                      float aspectRatio,
                                      float near,
                                      float far);

////////////////////////////////////////
void txGenOrthographicProjectionMatrix(TXmat4 m,
                                       float width,
                                       float height,
                                       float near,
                                       float far);

////////////////////////////////////////
void txGenLookAtMatrix(TXmat4 m, TXvec3 eye, TXvec3 target, TXvec3 up);

////////////////////////////////////////
void txGenViewMatrix(TXmat4 m, TXvec3 xAxis,
                               TXvec3 yAxis,
                               TXvec3 zAxis,
                               TXvec3 eye);

////////////////////////////////////////
TX_FORCE_INLINE void txGenNormalMatrix(TXmat4 normalMatrix,
                                       TXmat4 modelViewMatrix)
{
    txMat4Invert(normalMatrix, modelViewMatrix);
    txMat4Transpose(normalMatrix, normalMatrix);
}

////////////////////////////////////////
TX_FORCE_INLINE void txMulMat4Vec4(TXvec4 res, TXmat4 mat, TXvec4 v)
{
    float v0 = v[0], v1 = v[1], v2 = v[2], v3 = v[3];

    res[0] = mat[0] * v0 + mat[4] * v1 + mat[8]  * v2 + mat[12] * v3;
    res[1] = mat[1] * v0 + mat[5] * v1 + mat[9]  * v2 + mat[13] * v3;
    res[2] = mat[2] * v0 + mat[6] * v1 + mat[10] * v2 + mat[14] * v3;
    res[3] = mat[3] * v0 + mat[7] * v1 + mat[11] * v2 + mat[15] * v3;
}

////////////////////////////////////////
TX_FORCE_INLINE void txMulMat4Vec3(TXvec3 res, TXmat4 mat, TXvec3 v, float w)
{
    float v0 = v[0], v1 = v[1], v2 = v[2];

    res[0] = mat[0] * v0 + mat[4] * v1 + mat[8]  * v2 + mat[12] * w;
    res[1] = mat[1] * v0 + mat[5] * v1 + mat[9]  * v2 + mat[13] * w;
    res[2] = mat[2] * v0 + mat[6] * v1 + mat[10] * v2 + mat[14] * w;
}

////////////////////////////////////////
/// Specification of a triangle that
/// CursedGL uses internally for things like
/// clipping, lighting calculations,
/// vertex-attribute interpolation, etc.
////////////////////////////////////////
struct TXtriangle {

    // Positions of vertices in clip space
    TXvec4 v0_pos;
    TXvec4 v1_pos;
    TXvec4 v2_pos;

    // Positions of vertices in object space
    TXvec4 v0_obj_pos;
    TXvec4 v1_obj_pos;
    TXvec4 v2_obj_pos;

    // First attribute, aka,
    // layout (location = 0)
    TXvec4 v0_attr0;
    TXvec4 v1_attr0;
    TXvec4 v2_attr0;

    // Second attribute, aka,
    // layout (location = 1)
    TXvec4 v0_attr1;
    TXvec4 v1_attr1;
    TXvec4 v2_attr1;

    // Third attribute, aka,
    // layout (location = 2)
    TXvec4 v0_attr2;
    TXvec4 v1_attr2;
    TXvec4 v2_attr2;
};
typedef struct TXtriangle TXtriangle_t;

////////////////////////////////////////
/// Finds the position at which a line
/// intersects a given plane and stores
/// it in intersection
////////////////////////////////////////
float txIntersectPlane(TXvec4 intersection,
                       TXvec3 pointOnPlane,
                       TXvec3 planeNormal,
                       TXvec4 lineBegin,
                       TXvec4 lineEnd);

////////////////////////////////////////
TX_FORCE_INLINE float txSignedShortestDistanceFromPointToPlane(TXvec3 point,
                                                               TXvec3 planeNormal,
                                                               TXvec3 pointOnPlane)
{
    return txVec3Dot(point, planeNormal) - txVec3Dot(planeNormal, pointOnPlane);
}

////////////////////////////////////////
/// Returns the number of triangles that
/// will be produced by clipping the given
/// triangle against a given plane. Resulting
/// triangles are stored in tri{0,1}_out parameters
////////////////////////////////////////
int txClipTriangleAgainstPlane(TXtriangle_t* tri0_out,
                               TXtriangle_t* tri1_out,
                               TXvec3 pointOnPlane,
                               TXvec3 planeNormal,
                               TXtriangle_t* tri0_in);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
