// Copyright (C) 2023 saccharineboi

#include "transform.h"
#include <unistd.h>

////////////////////////////////////////
/// See https://www.gabrielgambetta.com/computer-graphics-from-scratch/11-clipping.html
/// to understand how clipping algorithm works
////////////////////////////////////////
int txClipTriangleAgainstPlane(TXtriangle_t* tri0_out,
                               TXtriangle_t* tri1_out,
                               TXvec3 pointOnPlane,
                               TXvec3 planeNormal,
                               TXtriangle_t* tri0_in)
{
    TXvec3 planeNormalNormalized;
    txVec3Normalize(planeNormalNormalized, planeNormal);

    int numInsidePoints  = 0;
    int numOutsidePoints = 0;

    float d0 = txSignedShortestDistanceFromPointToPlane(tri0_in->v0_pos,
                                                        planeNormalNormalized,
                                                        pointOnPlane);
    float d1 = txSignedShortestDistanceFromPointToPlane(tri0_in->v1_pos,
                                                        planeNormalNormalized,
                                                        pointOnPlane);
    float d2 = txSignedShortestDistanceFromPointToPlane(tri0_in->v2_pos,
                                                        planeNormalNormalized,
                                                        pointOnPlane);

    TXvec4 insidePoints[3];
    TXvec4 outsidePoints[3];

    if (d0 >= 0.0f) {
        txVec4Copy(insidePoints[numInsidePoints], tri0_in->v0_pos);
        ++numInsidePoints;
    }
    else {
        txVec4Copy(outsidePoints[numOutsidePoints], tri0_in->v0_pos);
        ++numOutsidePoints;
    }

    if (d1 >= 0.0f) {
        txVec4Copy(insidePoints[numInsidePoints], tri0_in->v1_pos);
        ++numInsidePoints;
    }
    else {
        txVec4Copy(outsidePoints[numOutsidePoints], tri0_in->v1_pos);
        ++numOutsidePoints;
    }

    if (d2 >= 0.0f) {
        txVec4Copy(insidePoints[numInsidePoints], tri0_in->v2_pos);
        ++numInsidePoints;
    }
    else {
        txVec4Copy(outsidePoints[numOutsidePoints], tri0_in->v2_pos);
        ++numOutsidePoints;
    }

    if (numInsidePoints == 0)
        return 0;
    else if (numInsidePoints == 3) {
        txVec4Copy(tri0_out->v0_pos, tri0_in->v0_pos);
        txVec4Copy(tri0_out->v1_pos, tri0_in->v1_pos);
        txVec4Copy(tri0_out->v2_pos, tri0_in->v2_pos);

        txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v0_obj_pos);
        txVec4Copy(tri0_out->v1_obj_pos, tri0_in->v1_obj_pos);
        txVec4Copy(tri0_out->v2_obj_pos, tri0_in->v2_obj_pos);
        return 1;
    }
    else if (numInsidePoints == 1 && numOutsidePoints == 2) {
        txVec4Copy(tri0_out->v0_pos, insidePoints[0]);

        float t0 = txIntersectPlane(tri0_out->v1_pos,
                                    pointOnPlane,
                                    planeNormalNormalized,
                                    insidePoints[0],
                                    outsidePoints[0]);

        float t1 = txIntersectPlane(tri0_out->v2_pos,
                                    pointOnPlane,
                                    planeNormalNormalized,
                                    insidePoints[0],
                                    outsidePoints[1]);

        TXvec4 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
        if (d0 >= 0.0f) {
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v0_obj_pos);
            txVec4Copy(tmp0, tri0_in->v1_obj_pos);
            txVec4Lerp(tri0_out->v1_obj_pos, tri0_in->v0_obj_pos, tmp0, t0);
            txVec4Copy(tmp0, tri0_in->v2_obj_pos);
            txVec4Lerp(tri0_out->v2_obj_pos, tri0_in->v0_obj_pos, tmp0, t1);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v0_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v0_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v0_attr2);

            txVec4Copy(tmp0, tri0_in->v1_attr0);
            txVec4Copy(tmp1, tri0_in->v1_attr1);
            txVec4Copy(tmp2, tri0_in->v1_attr2);

            txVec4Lerp(tri0_out->v1_attr0, tri0_in->v0_attr0, tmp0, t0);
            txVec4Lerp(tri0_out->v1_attr1, tri0_in->v0_attr1, tmp1, t0);
            txVec4Lerp(tri0_out->v1_attr2, tri0_in->v0_attr2, tmp2, t0);

            txVec4Copy(tmp0, tri0_in->v2_attr0);
            txVec4Copy(tmp1, tri0_in->v2_attr1);
            txVec4Copy(tmp2, tri0_in->v2_attr2);

            txVec4Lerp(tri0_out->v2_attr0, tri0_in->v0_attr0, tmp0, t1);
            txVec4Lerp(tri0_out->v2_attr1, tri0_in->v0_attr1, tmp1, t1);
            txVec4Lerp(tri0_out->v2_attr2, tri0_in->v0_attr2, tmp2, t1);
        }
        else if (d1 >= 0.0f) {
            txVec4Copy(tmp0, tri0_in->v0_obj_pos);
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v1_obj_pos);
            txVec4Copy(tmp3, tri0_in->v1_obj_pos);
            txVec4Lerp(tri0_out->v1_obj_pos, tmp3, tmp0, t0);
            txVec4Copy(tmp0, tri0_in->v2_obj_pos);
            txVec4Lerp(tri0_out->v2_obj_pos, tmp3, tmp0, t1);

            txVec4Copy(tmp0, tri0_in->v0_attr0);
            txVec4Copy(tmp1, tri0_in->v0_attr1);
            txVec4Copy(tmp2, tri0_in->v0_attr2);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v1_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v1_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v1_attr2);

            txVec4Copy(tmp3, tri0_in->v1_attr0);
            txVec4Copy(tmp4, tri0_in->v1_attr1);
            txVec4Copy(tmp5, tri0_in->v1_attr2);

            txVec4Lerp(tri0_out->v1_attr0, tmp3, tmp0, t0);
            txVec4Lerp(tri0_out->v1_attr1, tmp4, tmp1, t0);
            txVec4Lerp(tri0_out->v1_attr2, tmp5, tmp2, t0);

            txVec4Copy(tmp0, tri0_in->v2_attr0);
            txVec4Copy(tmp1, tri0_in->v2_attr1);
            txVec4Copy(tmp2, tri0_in->v2_attr2);

            txVec4Lerp(tri0_out->v2_attr0, tmp3, tmp0, t1);
            txVec4Lerp(tri0_out->v2_attr1, tmp4, tmp1, t1);
            txVec4Lerp(tri0_out->v2_attr2, tmp5, tmp2, t1);
        }
        else {
            txVec4Copy(tmp0, tri0_in->v0_obj_pos);
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v2_obj_pos);
            txVec4Copy(tmp3, tri0_in->v1_obj_pos);
            txVec4Lerp(tri0_out->v1_obj_pos, tri0_in->v2_obj_pos, tmp0, t0);
            txVec4Lerp(tri0_out->v2_obj_pos, tri0_in->v2_obj_pos, tmp3, t1);

            txVec4Copy(tmp0, tri0_in->v0_attr0);
            txVec4Copy(tmp1, tri0_in->v0_attr1);
            txVec4Copy(tmp2, tri0_in->v0_attr2);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v2_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v2_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v2_attr2);

            txVec4Copy(tmp3, tri0_in->v1_attr0);
            txVec4Copy(tmp4, tri0_in->v1_attr1);
            txVec4Copy(tmp5, tri0_in->v1_attr2);

            txVec4Lerp(tri0_out->v1_attr0, tri0_in->v2_attr0, tmp0, t0);
            txVec4Lerp(tri0_out->v1_attr1, tri0_in->v2_attr1, tmp1, t0);
            txVec4Lerp(tri0_out->v1_attr2, tri0_in->v2_attr2, tmp2, t0);

            txVec4Lerp(tri0_out->v2_attr0, tri0_in->v2_attr0, tmp3, t1);
            txVec4Lerp(tri0_out->v2_attr1, tri0_in->v2_attr1, tmp4, t1);
            txVec4Lerp(tri0_out->v2_attr2, tri0_in->v2_attr2, tmp5, t1);
        }

        return 1;
    }
    else if (numInsidePoints == 2 && numOutsidePoints == 1) {
        txVec4Copy(tri0_out->v0_pos, insidePoints[0]);
        txVec4Copy(tri0_out->v1_pos, insidePoints[1]);
        float t0 = txIntersectPlane(tri0_out->v2_pos,
                                    pointOnPlane,
                                    planeNormalNormalized,
                                    insidePoints[1],
                                    outsidePoints[0]);

        TXvec4 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        if (d0 < 0.0f) {
            txVec4Copy(tmp6, tri0_in->v0_obj_pos);
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v1_obj_pos);
            txVec4Copy(tmp7, tri0_in->v2_obj_pos);
            txVec4Copy(tri0_out->v1_obj_pos, tri0_in->v2_obj_pos);
            txVec4Lerp(tri0_out->v2_obj_pos, tmp7, tmp6, t0);

            txVec4Copy(tmp0, tri0_in->v0_attr0);
            txVec4Copy(tmp1, tri0_in->v0_attr1);
            txVec4Copy(tmp2, tri0_in->v0_attr2);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v1_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v1_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v1_attr2);

            txVec4Copy(tmp3, tri0_in->v2_attr0);
            txVec4Copy(tmp4, tri0_in->v2_attr1);
            txVec4Copy(tmp5, tri0_in->v2_attr2);

            txVec4Copy(tri0_out->v1_attr0, tri0_in->v2_attr0);
            txVec4Copy(tri0_out->v1_attr1, tri0_in->v2_attr1);
            txVec4Copy(tri0_out->v1_attr2, tri0_in->v2_attr2);

            txVec4Lerp(tri0_out->v2_attr0, tmp3, tmp0, t0);
            txVec4Lerp(tri0_out->v2_attr1, tmp4, tmp1, t0);
            txVec4Lerp(tri0_out->v2_attr2, tmp5, tmp2, t0);
        }
        else if (d1 < 0.0f) {
            txVec4Copy(tmp6, tri0_in->v1_obj_pos);
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v0_obj_pos);
            txVec4Copy(tmp7, tri0_out->v2_obj_pos);
            txVec4Copy(tri0_out->v1_obj_pos, tri0_in->v2_obj_pos);
            txVec4Lerp(tri0_out->v2_obj_pos, tmp7, tmp6, t0);

            txVec4Copy(tmp0, tri0_in->v1_attr0);
            txVec4Copy(tmp1, tri0_in->v1_attr1);
            txVec4Copy(tmp2, tri0_in->v1_attr2);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v0_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v0_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v0_attr2);

            txVec4Copy(tmp3, tri0_out->v2_attr0);
            txVec4Copy(tmp4, tri0_out->v2_attr1);
            txVec4Copy(tmp5, tri0_out->v2_attr2);

            txVec4Copy(tri0_out->v1_attr0, tri0_in->v2_attr0);
            txVec4Copy(tri0_out->v1_attr1, tri0_in->v2_attr1);
            txVec4Copy(tri0_out->v1_attr2, tri0_in->v2_attr2);

            txVec4Lerp(tri0_out->v2_attr0, tmp3, tmp0, t0);
            txVec4Lerp(tri0_out->v2_attr1, tmp4, tmp1, t0);
            txVec4Lerp(tri0_out->v2_attr2, tmp5, tmp2, t0);
        }
        else {
            txVec4Copy(tmp6, tri0_in->v2_obj_pos);
            txVec4Copy(tri0_out->v0_obj_pos, tri0_in->v0_obj_pos);
            txVec4Copy(tri0_out->v1_obj_pos, tri0_in->v1_obj_pos);
            txVec4Lerp(tri0_out->v2_obj_pos, tri0_out->v1_obj_pos, tmp6, t0);

            txVec4Copy(tmp0, tri0_in->v2_attr0);
            txVec4Copy(tmp1, tri0_in->v2_attr1);
            txVec4Copy(tmp2, tri0_in->v2_attr2);

            txVec4Copy(tri0_out->v0_attr0, tri0_in->v0_attr0);
            txVec4Copy(tri0_out->v0_attr1, tri0_in->v0_attr1);
            txVec4Copy(tri0_out->v0_attr2, tri0_in->v0_attr2);

            txVec4Copy(tri0_out->v1_attr0, tri0_in->v1_attr0);
            txVec4Copy(tri0_out->v1_attr1, tri0_in->v1_attr1);
            txVec4Copy(tri0_out->v1_attr2, tri0_in->v1_attr2);

            txVec4Lerp(tri0_out->v2_attr0, tri0_out->v1_attr0, tmp0, t0);
            txVec4Lerp(tri0_out->v2_attr1, tri0_out->v1_attr1, tmp1, t0);
            txVec4Lerp(tri0_out->v2_attr2, tri0_out->v1_attr2, tmp2, t0);
        }

        txVec4Copy(tri1_out->v0_pos, tri0_out->v2_pos);
        txVec4Copy(tri1_out->v2_pos, tri0_out->v0_pos);
        float t1 = txIntersectPlane(tri1_out->v1_pos,
                                    pointOnPlane,
                                    planeNormalNormalized,
                                    insidePoints[0],
                                    outsidePoints[0]);

        txVec4Copy(tri1_out->v0_attr0, tri0_out->v2_attr0);
        txVec4Copy(tri1_out->v0_attr1, tri0_out->v2_attr1);
        txVec4Copy(tri1_out->v0_attr2, tri0_out->v2_attr2);

        txVec4Copy(tri1_out->v2_attr0, tri0_out->v0_attr0);
        txVec4Copy(tri1_out->v2_attr1, tri0_out->v0_attr1);
        txVec4Copy(tri1_out->v2_attr2, tri0_out->v0_attr2);

        txVec4Copy(tri1_out->v0_obj_pos, tri0_out->v2_obj_pos);
        txVec4Copy(tri1_out->v2_obj_pos, tri0_out->v0_obj_pos);
        txVec4Lerp(tri1_out->v1_obj_pos, tri0_out->v0_obj_pos, tmp6, t1);

        txVec4Lerp(tri1_out->v1_attr0, tri0_out->v0_attr0, tmp0, t1);
        txVec4Lerp(tri1_out->v1_attr1, tri0_out->v0_attr1, tmp1, t1);
        txVec4Lerp(tri1_out->v1_attr2, tri0_out->v0_attr2, tmp2, t1);

        return 2;
    }
    return 0;
}

////////////////////////////////////////
float txIntersectPlane(TXvec4 intersection,
                       TXvec3 pointOnPlane,
                       TXvec3 planeNormal,
                       TXvec4 lineBegin,
                       TXvec4 lineEnd)
{
    float planeDot = txVec3Dot(planeNormal, pointOnPlane);
    float ad = txVec3Dot(lineBegin, planeNormal);
    float bd = txVec3Dot(lineEnd, planeNormal);
    float t = (planeDot - ad) / (bd - ad);
    txVec4Lerp(intersection, lineBegin, lineEnd, t);
    return t;
}

////////////////////////////////////////
void txGenPerspectiveProjectionMatrix(TXmat4 m,
                                      float fovy,
                                      float aspectRatio,
                                      float near,
                                      float far)
{
    float tangent = tanf(fovy * 0.5f);
    float height = near * tangent;
    float width = height * aspectRatio;

    m[0]  = near / width;
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;

    m[4]  = 0.0f;
    m[5]  = near / height;
    m[6]  = 0.0f;
    m[7]  = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = -(far + near) / (far - near);
    m[11] = -1.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = (-2.0f * near * far) / (far - near);
    m[15] = 0.0f;
}

////////////////////////////////////////
void txGenOrthographicProjectionMatrix(TXmat4 m,
                                       float width,
                                       float height,
                                       float near,
                                       float far)
{
    m[0]  = 1.0f / width;
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;

    m[4]  = 0.0f;
    m[5]  = 1.0f / height;
    m[6]  = 0.0f;
    m[7]  = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = -2.0f / (far - near);
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -(far + near) / (far - near);
    m[15] = 1.0f;
}

////////////////////////////////////////
void txGenLookAtMatrix(TXmat4 m, TXvec3 eye, TXvec3 target, TXvec3 up)
{
    TXvec3 zAxis;
    txVec3Sub(zAxis, eye, target);
    txVec3Normalize(zAxis, zAxis);

    TXvec3 xAxis;
    txVec3Cross(xAxis, up, zAxis);
    txVec3Normalize(xAxis, xAxis);

    TXvec3 yAxis;
    txVec3Cross(yAxis, zAxis, xAxis);
    txVec3Normalize(yAxis, yAxis);

    m[0]  = xAxis[0];
    m[1]  = yAxis[0];
    m[2]  = zAxis[0];
    m[3]  = 0.0f;

    m[4]  = xAxis[1];
    m[5]  = yAxis[1];
    m[6]  = zAxis[1];
    m[7]  = 0.0f;

    m[8]  = xAxis[2];
    m[9]  = yAxis[2];
    m[10] = zAxis[2];
    m[11] = 0.0f;

    m[12] = -txVec3Dot(xAxis, eye);
    m[13] = -txVec3Dot(yAxis, eye);
    m[14] = -txVec3Dot(zAxis, eye);
    m[15] = 1.0f;
}

////////////////////////////////////////
void txGenViewMatrix(TXmat4 m, TXvec3 xAxis,
                               TXvec3 yAxis,
                               TXvec3 zAxis,
                               TXvec3 eye)
{
    m[0]  = xAxis[0];
    m[1]  = yAxis[0];
    m[2]  = zAxis[0];
    m[3]  = 0.0f;

    m[4]  = xAxis[1];
    m[5]  = yAxis[1];
    m[6]  = zAxis[1];
    m[7]  = 0.0f;

    m[8]  = xAxis[2];
    m[9]  = yAxis[2];
    m[10] = zAxis[2];
    m[11] = 0.0f;

    m[12] = -txVec3Dot(xAxis, eye);
    m[13] = -txVec3Dot(yAxis, eye);
    m[14] = -txVec3Dot(zAxis, eye);
    m[15] = 1.0f;
}
