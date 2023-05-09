// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "vec.h"
#include "mat3.h"
#include "mat4.h"
#include "common.h"
#include "framebuffer.h"
#include "transform.h"
#include "lights.h"
#include "error.h"

#include <unistd.h>

////////////////////////////////////////
#define TX_LINE_BIAS 0.5f
#define TX_FB_BIAS   0.01f

////////////////////////////////////////
/// Specifies which face(s) of a triangle
/// must be culled.
///
/// It's RECOMMENDED to turn this on as
/// it will save a LOT of CPU cycles, especially
/// when you're rendering a lot of vertices
///
/// By default face culling is
/// turned off. In order to enable it
/// call txEnable(TX_CULL_FACE). Default
/// face to be culled is TX_BACK.
////////////////////////////////////////
enum TXcullFace { TX_NONE,
                  TX_FRONT,
                  TX_BACK,
                  TX_FRONT_AND_BACK };

////////////////////////////////////////
/// Specifies the winding order of the
/// triangle
/// TX_CW  : clockwise
/// TX_CCW : counter-clockwise
///
/// By default CursedGL expects
/// triangles in counter-clockwise order
////////////////////////////////////////
enum TXwindOrder { TX_CW, TX_CCW };

////////////////////////////////////////
void txFrontFace(enum TXwindOrder frontFace);

////////////////////////////////////////
enum TXwindOrder txGetFrontFace();

////////////////////////////////////////
void txCullFace(enum TXcullFace cullFace);

////////////////////////////////////////
enum TXcullFace txGetCullFace();

////////////////////////////////////////
/// Following 4 functions:
///
/// * txColor4f
/// * txColor3f
/// * txColor4fv
/// * txColor3fv
///
/// are only useful while rendering during
/// TX_UNLIT mode. In fact this is how you
/// specify the color the mesh will be rendered
/// with while you're in TX_UNLIT mode.
///
/// In contrast to OpenGL, you can't specify
/// the color of each vertex using this method,
/// for that you'll need to embed those colors
/// into your vertices and then choose one of
/// the suitable predefined VAO configurations specified by
/// TXvertexInfo. Check the examples/ folder
/// for C source files that demonstrate this functionality
////////////////////////////////////////

////////////////////////////////////////
void txColor4fv(TXvec4 color);

////////////////////////////////////////
TX_FORCE_INLINE void txColor3fv(TXvec3 color)
{
    TXvec4 c = { color[0], color[1], color[2], 1.0f };
    txColor4fv(c);
}

////////////////////////////////////////
void txColor4f(float r, float g, float b, float a);

////////////////////////////////////////
#define txColor3f(r, g, b) txColor4f(r, g, b, 1.0f)

////////////////////////////////////////
void txGetColor4fv(TXvec4 color);

////////////////////////////////////////
void txGetColor4f(float* r, float* g, float* b, float* a);

////////////////////////////////////////
float* txGetColorPtr();

////////////////////////////////////////
/// Computes perspective projection
/// to be used by the rasterizer to transform
/// vertices before rendering
////////////////////////////////////////
void txPerspective(float fovy,
                   float aspectRatio,
                   float near,
                   float far);

////////////////////////////////////////
/// Computes orthographic projection
/// to be used by the rasterizer to transform
/// vertices before rendering
////////////////////////////////////////
void txOrtho(float width,
             float height,
             float near,
             float far);

////////////////////////////////////////
void txSetProjectionMatrix(TXmat4 projectionMatrix);

////////////////////////////////////////
float* txGetProjectionMatrix();

////////////////////////////////////////
void txSetModelViewMatrix(TXmat4 modelViewMatrix);

////////////////////////////////////////
float* txGetModelViewMatrix();

////////////////////////////////////////
void txSetNormalMatrix(TXmat4 normalMatrix);

////////////////////////////////////////
float* txGetNormalMatrix();

////////////////////////////////////////
void txSetTextureMatrix(TXmat4 textureMatrix);

////////////////////////////////////////
float* txGetTextureMatrix();

////////////////////////////////////////
void txSetLightMatrix(TXmat4 lightMatrix);

////////////////////////////////////////
float* txGetLightMatrix();

////////////////////////////////////////
/// Blocks in terminals may have widths
/// different from their heights. Setting
/// widthMultiplier ensures objects don't look
/// distorted in the terminal
///
/// I've found widthMultiplier between
/// 2.0 and 2.5 to look best in most
/// terminals, but you should play with
/// this value if your rendering looks
/// distorted or weird
////////////////////////////////////////
void txSetWidthMultiplier(float widthMultiplier);

////////////////////////////////////////
float txGetWidthMultiplier();

////////////////////////////////////////
TX_FORCE_INLINE void txCopyTransform(enum TXmatrixType dst,
                                     enum TXmatrixType src)
{
    float* dstMatrix = NULL;
    float* srcMatrix = NULL;

    switch (src) {
        case TX_PROJECTION:
            srcMatrix = txGetProjectionMatrix();
            break;
        case TX_MODELVIEW:
            srcMatrix = txGetModelViewMatrix();
            break;
        case TX_NORMAL:
            srcMatrix = txGetNormalMatrix();
            break;
        case TX_TEXTURE:
            srcMatrix = txGetTextureMatrix();
            break;
        case TX_LIGHT:
            srcMatrix = txGetLightMatrix();
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txCopyTransform: source matrix type (%d) is invalid", src);
            break;
    }

    switch (dst) {
        case TX_PROJECTION:
            dstMatrix = txGetProjectionMatrix();
            break;
        case TX_MODELVIEW:
            dstMatrix = txGetModelViewMatrix();
            break;
        case TX_NORMAL:
            dstMatrix = txGetNormalMatrix();
            break;
        case TX_TEXTURE:
            dstMatrix = txGetTextureMatrix();
            break;
        case TX_LIGHT:
            dstMatrix = txGetLightMatrix();
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txCopyTransform: destination matrix type (%d) is invalid", dst);
            break;
    }

    if (srcMatrix && dstMatrix)
        txMat4Copy(dstMatrix, srcMatrix);
}

////////////////////////////////////////
TX_FORCE_INLINE void txTranslate3f(float tx, float ty, float tz)
{
    TXvec3 translation = { tx, ty, tz };
    switch (txGetMatrixMode()) {
        case TX_PROJECTION:
            txTranslateMat4(txGetProjectionMatrix(), translation);
            break;
        case TX_MODELVIEW:
            txTranslateMat4(txGetModelViewMatrix(), translation);
            break;
        case TX_NORMAL:
            txTranslateMat4(txGetNormalMatrix(), translation);
            break;
        case TX_TEXTURE:
            txTranslateMat4(txGetTextureMatrix(), translation);
            break;
        case TX_LIGHT:
            txTranslateMat4(txGetLightMatrix(), translation);
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txTranslate3f: matrix mode is invalid");
            break;
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txTranslate2f(float tx, float ty)
{
    txTranslate3f(tx, ty, 0.0f);
}

////////////////////////////////////////
TX_FORCE_INLINE void txTranslate3fv(TXvec3 t)
{
    txTranslate3f(t[0], t[1], t[2]);
}

////////////////////////////////////////
TX_FORCE_INLINE void txTranslate2fv(TXvec2 t)
{
    txTranslate3f(t[0], t[1], 0.0f);
}

////////////////////////////////////////
TX_FORCE_INLINE void txRotate4f(float rads, float xAxis,
                                            float yAxis,
                                            float zAxis)
{
    switch (txGetMatrixMode()) {
        case TX_PROJECTION:
            txRotateMat4(txGetProjectionMatrix(), rads, xAxis, yAxis, zAxis);
            break;
        case TX_MODELVIEW:
            txRotateMat4(txGetModelViewMatrix(), rads, xAxis, yAxis, zAxis);
            txGenNormalMatrix(txGetNormalMatrix(), txGetModelViewMatrix());
            break;
        case TX_NORMAL:
            txRotateMat4(txGetNormalMatrix(), rads, xAxis, yAxis, zAxis);
            break;
        case TX_TEXTURE:
            txRotateMat4(txGetTextureMatrix(), rads, xAxis, yAxis, zAxis);
            break;
        case TX_LIGHT:
            txRotateMat4(txGetLightMatrix(), rads, xAxis, yAxis, zAxis);
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txRotate4f: matrix mode is invalid");
            break;
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txRotate3fv(float rads, TXvec3 axis)
{
    txRotate4f(rads, axis[0], axis[1], axis[2]);
}

////////////////////////////////////////
TX_FORCE_INLINE void txScale3f(float sx, float sy, float sz)
{
    TXvec3 scale = { sx, sy, sz };
    switch (txGetMatrixMode()) {
        case TX_PROJECTION:
            txScaleMat4(txGetProjectionMatrix(), scale);
            break;
        case TX_MODELVIEW:
            txScaleMat4(txGetModelViewMatrix(), scale);
            txGenNormalMatrix(txGetNormalMatrix(), txGetModelViewMatrix());
            break;
        case TX_NORMAL:
            txScaleMat4(txGetNormalMatrix(), scale);
            break;
        case TX_TEXTURE:
            txScaleMat4(txGetTextureMatrix(), scale);
            break;
        case TX_LIGHT:
            txScaleMat4(txGetLightMatrix(), scale);
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txScale3f: matrix mode is invalid");
            break;
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txScale2f(float sx, float sy)
{
    txScale3f(sx, sy, 1.0f);
}

////////////////////////////////////////
TX_FORCE_INLINE void txScale3fv(TXvec3 s)
{
    txScale3f(s[0], s[1], s[2]);
}

////////////////////////////////////////
TX_FORCE_INLINE void txScale2fv(TXvec2 s)
{
    txScale3f(s[0], s[1], 1.0f);
}

////////////////////////////////////////
TX_FORCE_INLINE void txLoadIdentity()
{
    switch (txGetMatrixMode()) {
        case TX_PROJECTION:
            txMat4Identity(txGetProjectionMatrix());
            break;
        case TX_MODELVIEW:
            txMat4Identity(txGetModelViewMatrix());
            break;
        case TX_NORMAL:
            txMat4Identity(txGetNormalMatrix());
            break;
        case TX_TEXTURE:
            txMat4Identity(txGetTextureMatrix());
            break;
        case TX_LIGHT:
            txMat4Identity(txGetLightMatrix());
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txLoadIdentity: matrix mode is invalid");
            break;
    }
}

////////////////////////////////////////
TX_FORCE_INLINE void txConvertToViewSpace(TXvec4 dst, TXvec4 src)
{
    txMulMat4Vec4(dst, txGetModelViewMatrix(), src);
}

////////////////////////////////////////
TX_FORCE_INLINE void txConvertToClipSpace(TXvec4 dst, TXvec4 src)
{
    txMulMat4Vec4(dst, txGetProjectionMatrix(), src);
}

////////////////////////////////////////
TX_FORCE_INLINE void txConvertToWindowSpace(TXvec3 dst, TXvec4 src)
{
    TXvec4 ndcVec;
    txVec4DivideByW(ndcVec, src);

    float fbWidth  = (float)txGetFramebufferWidth();
    float fbHeight = (float)txGetFramebufferHeight();

    dst[0] = (fbWidth  / 2.0f) * ( ndcVec[0] + 1.0f);
    dst[1] = (fbHeight / 2.0f) * (-ndcVec[1] + 1.0f);
    dst[2] = (ndcVec[2] + 1.0f) / 2.0f;
}

////////////////////////////////////////
/// Returns true if a given point defined
/// by (i, j) is inside a triangle defined
/// by vertices { v0, v1, v2 }, false otherwise.
///
/// Also returns the barycentric coordinates IFF
/// the point is inside the triangle
///
/// All vectors are in window coordinates,
/// thus all computations are happening in screen-space.
/// At this point, txConvertToWindowSpace has already run
///
/// For a quick overview of how this algorithm works,
/// check out https://youtu.be/HYAgJN3x4GA
////////////////////////////////////////
bool txIsPointInTriangle(int i,
                         int j,
                         TXvec3 v0,
                         TXvec3 v1,
                         TXvec3 v2,
                         TXvec3 weights);

////////////////////////////////////////
/// Returns true if a point defined by
/// (i, j) is on a line defined by
/// vertices { v0, v1 }, false otherwise
///
/// All vectors are in window coordinates,
/// thus all computations are happening in screen-space.
/// At this point, txConvertToWindowSpace has already run
////////////////////////////////////////
bool txIsPointInLine(int i,
                     int j,
                     TXvec3 v0,
                     TXvec3 v1);

////////////////////////////////////////
/// Rasterizes the given point defined by v0
/// in world-space
////////////////////////////////////////
void txDrawPoint(TXvec4 v0);

////////////////////////////////////////
/// Finds the z-position of a given pixel
/// identified by (i, j) on a line defined
/// by points { viewport_v0, viewport_v1 }
////////////////////////////////////////
TX_FORCE_INLINE float txInterpolateLineDepth(int i,
                                             int j,
                                             TXvec3 viewport_v0,
                                             TXvec3 viewport_v1)
{
    float lineLength = txVec2Dist(viewport_v0, viewport_v1);
    TXvec2 point = { (float)i, (float)j };
    float pointDistance = txVec2Dist(viewport_v0, point);
    float interpolatedZ = viewport_v0[2] + (pointDistance / lineLength) * (viewport_v1[2] - viewport_v0[2]);
    return interpolatedZ;
}

////////////////////////////////////////
/// Rasterizes the line defined by { v0, v1 }
/// in world-space
////////////////////////////////////////
void txDrawLine(TXvec4 v0, TXvec4 v1);

////////////////////////////////////////
TX_FORCE_INLINE void txDrawLineStrip(TXvec4* vertices, int numVertices)
{
    for (int i = 0; i < numVertices - 1; ++i)
        txDrawLine(vertices[i], vertices[i + 1]);
}

////////////////////////////////////////
TX_FORCE_INLINE void txDrawLineLoop(TXvec4* vertices, int numVertices)
{
    txDrawLineStrip(vertices, numVertices);
    if (numVertices > 2)
        txDrawLine(vertices[numVertices - 1], vertices[0]);
}

////////////////////////////////////////
bool txShouldCullFace(TXvec3 view_v0,
                      TXvec3 view_v1,
                      TXvec3 view_v2);

////////////////////////////////////////
/// Interpolates the given vertex values
/// in v0, v1, v2 with given weights and
/// stores the result in res.
///
/// Weights are barycentric coordinates
/// of a triangle defined by v0, v1, v2
/// for some point P on the triangle
///
/// This is perspective-correct
/// interpolation, and in fact currently
/// the only way to perform interpolation
/// in CursedGL.
///
/// I'm not sure if I should also provide
/// perspective-incorrect interpolation
/// (see noperspective qualifier in opengl,
/// link: https://www.geeks3d.com/20130514/opengl-interpolation-qualifiers-glsl-tutorial/)
/// for vertex attributes, so far I've not
/// encountered a use-case for this
/// (other than emulating PS1-style
/// affine texture mapping)
////////////////////////////////////////
TX_FORCE_INLINE void txInterpolateVertexElement(TXvec4 res,
                                                TXvec4 v0,
                                                TXvec4 v1,
                                                TXvec4 v2,
                                                TXvec3 weights,
                                                TXvec3 zValues,
                                                float interpolatedZ)
{
    float zw0 = zValues[0] * weights[0];
    float zw1 = zValues[1] * weights[1];
    float zw2 = zValues[2] * weights[2];

    res[0] = v0[0] * zw0 + v1[0] * zw1 + v2[0] * zw2;
    res[1] = v0[1] * zw0 + v1[1] * zw1 + v2[1] * zw2;
    res[2] = v0[2] * zw0 + v1[2] * zw1 + v2[2] * zw2;
    res[3] = v0[3] * zw0 + v1[3] * zw1 + v2[3] * zw2;

    txVec4ScalarDiv(res, res, interpolatedZ);
}

////////////////////////////////////////
/// Instead of interpolating, this function
/// simply takes three neighboring vertices
/// and stores the averages of their values
/// in res.
////////////////////////////////////////
TX_FORCE_INLINE void txAverageVertexElement(TXvec4 res,
                                            TXvec4 v0,
                                            TXvec4 v1,
                                            TXvec4 v2)
{
    res[0] = (v0[0] + v1[0] + v2[0]) / 3.0f;
    res[1] = (v0[1] + v1[1] + v2[1]) / 3.0f;
    res[2] = (v0[2] + v1[2] + v2[2]) / 3.0f;
    res[3] = (v0[3] + v1[3] + v2[3]) / 3.0f;
}

////////////////////////////////////////
/// Converts a given set of vertices from their
/// own space to a custom space specified by the given
/// matrix
////////////////////////////////////////
TX_FORCE_INLINE void txConvertToCustomSpace(TXvec4 dst0, TXvec4 dst1, TXvec4 dst2,
                                            TXmat4 mat,
                                            TXvec4 src0, TXvec4 src1, TXvec4 src2)
{
    txMulMat4Vec4(dst0, mat, src0);
    txMulMat4Vec4(dst1, mat, src1);
    txMulMat4Vec4(dst2, mat, src2);
}

////////////////////////////////////////
/// Rasterizes the given triangle defined
/// by vertices { v0, v1, v2 } in world-space
////////////////////////////////////////
void txDrawTriangle(TXvec4 v0[],
                    TXvec4 v1[],
                    TXvec4 v2[],
                    enum TXvertexInfo vertexInfo);

////////////////////////////////////////
/// Rasterizes the given quad defined by four
/// vertices { v0, v1, v2, v3 } in world-space
////////////////////////////////////////
TX_FORCE_INLINE void txDrawQuad(TXvec4 v0[],
                                TXvec4 v1[],
                                TXvec4 v2[],
                                TXvec4 v3[],
                                enum TXvertexInfo vertexInfo)
{
    txDrawTriangle(v0, v1, v2, vertexInfo);
    txDrawTriangle(v0, v2, v3, vertexInfo);
}

////////////////////////////////////////
/// Rasterizes the given triangle strip
/// given by vertex array in vertices[]
/// where each vertex is in world-space
////////////////////////////////////////
TX_FORCE_INLINE void txDrawTriangleStrip(TXvec4* vertices[],
                                         int numVertices,
                                         enum TXvertexInfo vertexInfo)
{
    for (int i = 0; i < numVertices - 2; ++i) {
        if (!i || !(i % 2))
            txDrawTriangle(vertices[i + 2], vertices[i + 1], vertices[i], vertexInfo);
        else
            txDrawTriangle(vertices[i], vertices[i + 1], vertices[i + 2], vertexInfo);
    }
}

////////////////////////////////////////
/// Rasterizes the given triangle fan
/// given by vertex array in vertices[]
/// where each vertex is in world-space
////////////////////////////////////////
TX_FORCE_INLINE void txDrawTriangleFan(TXvec4* vertices[],
                                       int numVertices,
                                       enum TXvertexInfo vertexInfo)
{
    for (int i = 0; i < numVertices - 2; ++i)
        txDrawTriangle(vertices[0],
                       vertices[i + 1],
                       vertices[i + 2], vertexInfo);
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
