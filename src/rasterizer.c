// Copyright (C) 2023 saccharineboi

#include "rasterizer.h"
#include "error.h"

#include <string.h>
#include <notcurses/notcurses.h>

////////////////////////////////////////
/// This is the color that will be used
/// while rendering objects in
/// TX_UNLIT mode
////////////////////////////////////////
static TXvec4 rasterColor = TX_VEC4_ONE;

////////////////////////////////////////
//////////// MATRIX STACKS /////////////
////////////////////////////////////////

////////////////////////////////////////
static TXmat4 projectionMatrixStack[TX_PROJECTION_MATRIX_STACK_SIZE] = { TX_MAT4_IDENTITY };
static int currentProjectionMatrix;

////////////////////////////////////////
static TXmat4 modelViewMatrixStack[TX_MODELVIEW_MATRIX_STACK_SIZE] = { TX_MAT4_IDENTITY };
static int currentModelViewMatrix;

////////////////////////////////////////
static TXmat4 normalMatrixStack[TX_NORMAL_MATRIX_STACK_SIZE] = { TX_MAT4_IDENTITY };
static int currentNormalMatrix;

////////////////////////////////////////
static TXmat4 textureMatrixStack[TX_TEXTURE_MATRIX_STACK_SIZE] = { TX_MAT4_IDENTITY };
static int currentTextureMatrix;

////////////////////////////////////////
static TXmat4 lightMatrixStack[TX_LIGHT_MATRIX_STACK_SIZE] = { TX_MAT4_IDENTITY };
static int currentLightMatrix;

////////////////////////////////////////
/// See the comment above txSetWidthMultiplier
/// function in rasterizer.h
////////////////////////////////////////
static float widthMultiplier = 2.0f;

////////////////////////////////////////
/// See enum TXcullFace in rasterizer.h
////////////////////////////////////////
static int cullFace = TX_BACK;

////////////////////////////////////////
/// See enum TXwindOrder in rasterizer.h
////////////////////////////////////////
static int windOrder = TX_CCW;

////////////////////////////////////////
static int matrixMode = TX_MODELVIEW;

////////////////////////////////////////
/// See enum TXshadeModel in rasterizer.h
////////////////////////////////////////
static int shadeModel = TX_UNLIT;

////////////////////////////////////////
void txShadeModel(enum TXshadeModel model)
{
    shadeModel = model;
}

////////////////////////////////////////
enum TXshadeModel txGetShadeModel()
{
    return shadeModel;
}

////////////////////////////////////////
void txMatrixMode(enum TXmatrixType type)
{
    matrixMode = type;
}

////////////////////////////////////////
enum TXmatrixType txGetMatrixMode()
{
    return matrixMode;
}

////////////////////////////////////////
bool txPushMatrix()
{
    switch (matrixMode) {
        case TX_PROJECTION:
            if (currentProjectionMatrix < TX_PROJECTION_MATRIX_STACK_SIZE - 1) {
                ++currentProjectionMatrix;
                txMat4Copy(projectionMatrixStack[currentProjectionMatrix],
                           projectionMatrixStack[currentProjectionMatrix - 1]);
                return true;
            } else {
                txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: projection matrix stack is full");
            }
            break;
        case TX_MODELVIEW:
            if (currentModelViewMatrix < TX_MODELVIEW_MATRIX_STACK_SIZE - 1) {
                ++currentModelViewMatrix;
                txMat4Copy(modelViewMatrixStack[currentModelViewMatrix],
                           modelViewMatrixStack[currentModelViewMatrix - 1]);
                return true;
            } else {
                txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: modelview matrix stack is full");
            }
            break;
        case TX_NORMAL:
            if (currentNormalMatrix < TX_NORMAL_MATRIX_STACK_SIZE - 1) {
                ++currentNormalMatrix;
                txMat4Copy(normalMatrixStack[currentNormalMatrix],
                           normalMatrixStack[currentNormalMatrix - 1]);
                return true;
            } else {
                txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: normal matrix stack is full");
            }
            break;
        case TX_TEXTURE:
            if (currentTextureMatrix < TX_TEXTURE_MATRIX_STACK_SIZE - 1) {
                ++currentTextureMatrix;
                txMat4Copy(textureMatrixStack[currentTextureMatrix],
                           textureMatrixStack[currentTextureMatrix - 1]);
                return true;
            } else {
                txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: texture matrix stack is full");
            }
            break;
        case TX_LIGHT:
            if (currentLightMatrix < TX_LIGHT_MATRIX_STACK_SIZE - 1) {
                ++currentLightMatrix;
                txMat4Copy(lightMatrixStack[currentLightMatrix],
                           lightMatrixStack[currentLightMatrix - 1]);
                return true;
            } else {
                txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: light matrix stack is full");
            }
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] txPushMatrix: %d is invalid matrix mode", matrixMode);
            break;
    }
    return false;
}

////////////////////////////////////////
bool txPopMatrix()
{
    switch (matrixMode) {
        case TX_PROJECTION:
            if (currentProjectionMatrix > 0) {
                --currentProjectionMatrix;
                return true;
            }
            txOutputMessage(TX_WARNING, "[CursedGL] txPopMatrix: projection matrix stack is empty");
            break;
        case TX_MODELVIEW:
            if (currentModelViewMatrix > 0) {
                --currentModelViewMatrix;
                return true;
            }
            txOutputMessage(TX_WARNING, "[CursedGL] txPopMatrix: modelview matrix stack is empty");
            break;
        case TX_NORMAL:
            if (currentNormalMatrix > 0) {
                --currentNormalMatrix;
                return true;
            }
            txOutputMessage(TX_WARNING, "[CursedGL] txPopMatrix: normal matrix stack is empty");
            break;
        case TX_TEXTURE:
            if (currentTextureMatrix > 0) {
                --currentTextureMatrix;
                return true;
            }
            txOutputMessage(TX_WARNING, "[CursedGL] txPopMatrix: texture matrix stack is empty");
            break;
        case TX_LIGHT:
            if (currentLightMatrix > 0) {
                --currentLightMatrix;
                return true;
            }
            txOutputMessage(TX_WARNING, "[CursedGL] txPopMatrix: light matrix stack is empty");
            break;
    }
    return false;
}

////////////////////////////////////////
float txGetWidthMultiplier()
{
    return widthMultiplier;
}

////////////////////////////////////////
void txSetWidthMultiplier(float x)
{
    widthMultiplier = x;
}

////////////////////////////////////////
void txSetProjectionMatrix(TXmat4 matrix)
{
    txMat4Copy(projectionMatrixStack[currentProjectionMatrix], matrix);
}

////////////////////////////////////////
float* txGetProjectionMatrix()
{
    return projectionMatrixStack[currentProjectionMatrix];
}

////////////////////////////////////////
void txSetModelViewMatrix(TXmat4 matrix)
{
    txMat4Copy(modelViewMatrixStack[currentModelViewMatrix], matrix);
}

////////////////////////////////////////
float* txGetModelViewMatrix()
{
    return modelViewMatrixStack[currentModelViewMatrix];
}

////////////////////////////////////////
void txSetNormalMatrix(TXmat4 matrix)
{
    txMat4Copy(normalMatrixStack[currentNormalMatrix], matrix);
}

////////////////////////////////////////
float* txGetNormalMatrix()
{
    return normalMatrixStack[currentNormalMatrix];
}

////////////////////////////////////////
void txSetTextureMatrix(TXmat4 matrix)
{
    txMat4Copy(textureMatrixStack[currentTextureMatrix], matrix);
}

////////////////////////////////////////
float* txGetTextureMatrix()
{
    return textureMatrixStack[currentTextureMatrix];
}

////////////////////////////////////////
void txSetLightMatrix(TXmat4 matrix)
{
    txMat4Copy(lightMatrixStack[currentLightMatrix], matrix);
}

////////////////////////////////////////
float* txGetLightMatrix()
{
    return lightMatrixStack[currentLightMatrix];
}

////////////////////////////////////////
void txPerspective(float fovy,
                   float aspectRatio,
                   float near,
                   float far)
{
    txGenPerspectiveProjectionMatrix(projectionMatrixStack[currentProjectionMatrix],
                                     fovy,
                                     aspectRatio / widthMultiplier,
                                     near,
                                     far);
}

////////////////////////////////////////
void txOrtho(float width,
             float height,
             float near,
             float far)
{
    txGenOrthographicProjectionMatrix(projectionMatrixStack[currentProjectionMatrix],
                                      width * widthMultiplier,
                                      height,
                                      near,
                                      far);
}

////////////////////////////////////////
void txColor4fv(TXvec4 color)
{
    txVec4Copy(rasterColor, color);
}

////////////////////////////////////////
void txColor4f(float r, float g, float b, float a)
{
    rasterColor[0] = r;
    rasterColor[1] = g;
    rasterColor[2] = b;
    rasterColor[3] = a;
}

////////////////////////////////////////
void txGetColor4fv(TXvec4 color)
{
    txVec4Copy(color, rasterColor);
}

////////////////////////////////////////
void txGetColor4f(float* r, float* g, float* b, float* a)
{
    *r = rasterColor[0];
    *g = rasterColor[1];
    *b = rasterColor[2];
    *a = rasterColor[3];
}

////////////////////////////////////////
float* txGetColorPtr()
{
    return rasterColor;
}

////////////////////////////////////////
void txCullFace(enum TXcullFace face)
{
    cullFace = face;
}

////////////////////////////////////////
enum TXcullFace txGetCullFace()
{
    return cullFace;
}

////////////////////////////////////////
void txFrontFace(enum TXwindOrder frontFace)
{
    windOrder = frontFace;
}

////////////////////////////////////////
enum TXwindOrder txGetFrontFace()
{
    return windOrder;
}

////////////////////////////////////////
/// A single invocation of this function
/// corresponds to 3 invocations of a vertex
/// shader for a given triangle in a
/// typical hardware-accelerated graphics pipeline.
////////////////////////////////////////
TX_FORCE_INLINE void runVertexShader(enum TXvertexInfo vertexInfo,
                                     TXtriangle_t* tri,
                                     TXvec4 ss_v0, TXvec4 ss_v1, TXvec4 ss_v2,
                                     TXvec3 zValues,
                                     TXvec4 normal0, TXvec4 normal1, TXvec4 normal2,
                                     TXvec4 mvPos0,  TXvec4 mvPos1,  TXvec4 mvPos2)
{
    ////////////////////////////////////////
    // Once upon a time, the following 3 divisions
    // were simply not doable. I've pasted some links
    // for you to give you context and history behind
    // these divisions, and how the engineers in the past
    // dealt with them, and how modern hardware handles them.
    //
    // 1. https://youtu.be/x8TO-nrUtSI
    // 2. https://youtu.be/9Cw3K49Fffc
    // 3. https://youtu.be/Bz2ZEBRjm4w
    // 4. https://youtu.be/RyYEGdGwnFs
    // 5. https://youtu.be/1Dv2-cLAJXw
    // 6. https://gamma.cs.unc.edu/courses/graphics-s09/Zaferakis/hw6/
    // 7. https://web.archive.org/web/20050414151509/https://www.cs.unc.edu/~andrewz/comp236/hw5a/index.html
    // 8. https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
    // 9. https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation
    // 10. https://youtu.be/TuH7RDIDZN4
    // 11. https://www.cs.utexas.edu/users/fussell/courses/cs384g-fall2011/lectures/lecture07-Affine.pdf
    // 12. https://www.geeks3d.com/20130514/opengl-interpolation-qualifiers-glsl-tutorial/
    //
    // These divisions are crucial for properly interpolating
    // vertex attributes (one of which is texture coordinates)
    // in screen-space. In fact, Blinn-Phong algorithm won't
    // work without them since it requires interpolated
    // fragment positions to compute specular colors
    //
    // You may also wanna read this: https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.72.6546&rep=rep1&type=pdf
    ////////////////////////////////////////
    zValues[0] = -1.0f / tri->v0_pos[3];
    zValues[1] = -1.0f / tri->v1_pos[3];
    zValues[2] = -1.0f / tri->v2_pos[3];
    ////////////////////////////////////////

    txConvertToWindowSpace(ss_v0, tri->v0_pos);
    txConvertToWindowSpace(ss_v1, tri->v1_pos);
    txConvertToWindowSpace(ss_v2, tri->v2_pos);

    switch (vertexInfo) {
        case TX_POSITION_NORMAL:
            txConvertToCustomSpace(mvPos0, mvPos1, mvPos2,
                                   txGetModelViewMatrix(),
                                   tri->v0_obj_pos,
                                   tri->v1_obj_pos,
                                   tri->v2_obj_pos);
            txConvertToCustomSpace(normal0, normal1, normal2,
                                   txGetNormalMatrix(),
                                   tri->v0_attr0,
                                   tri->v1_attr0,
                                   tri->v2_attr0);
            break;
        case TX_POSITION_COLOR_NORMAL:
            txConvertToCustomSpace(mvPos0, mvPos1, mvPos2,
                                   txGetModelViewMatrix(),
                                   tri->v0_obj_pos,
                                   tri->v1_obj_pos,
                                   tri->v2_obj_pos);
            txConvertToCustomSpace(normal0, normal1, normal2,
                                   txGetNormalMatrix(),
                                   tri->v0_attr1,
                                   tri->v1_attr1,
                                   tri->v2_attr1);
            break;
        case TX_POSITION:
            break;
        case TX_POSITION_COLOR:
            break;
        case TX_POSITION_TEXCOORD:
        case TX_POSITION_COLOR_TEXCOORD:
        case TX_POSITION_NORMAL_TEXCOORD:
        case TX_POSITION_COLOR_NORMAL_TEXCOORD:
            txOutputMessage(TX_INFO, "[CursedGL] runVertexShader: given VAO configuration is currently not implemented");
            break;
        default:
            txOutputMessage(TX_WARNING, "[CursedGL] runVertexShader: given VOA configuration (%d) is invalid", vertexInfo);
            break;
    }
}

////////////////////////////////////////
/// Given vertexInfo (i.e. VAO configuration),
/// triangle tri, weights, zValues, and interpolatedZ,
/// run the fragment shader and the store result
/// in outputColor.
///
/// zValues is an array containing the inverted z-coordinates
/// of each vertex of the given triangle
///
/// interpolatedZ is the interpolated z-coordinate
/// of the current pixel
///
/// weights is an array containing the barycentric
/// coordinates of the current pixel in screen-space
///
/// To see how the above 3 values are computed see
/// renderTriangles in rasterizer.c
///
/// normal{0,1,2} are normals processed by the vertex
/// shader
///
/// mvPos{0,1,2} are positions of each vertex of a
/// triangle in model-view space (also processed
/// by the vertex shader)
////////////////////////////////////////
TX_FORCE_INLINE void runFragmentShader(enum TXvertexInfo vertexInfo,
                                       TXtriangle_t* tri,
                                       TXvec4 outputColor,
                                       TXvec3 weights,
                                       TXvec3 zValues,
                                       TXvec4 normal0, TXvec4 normal1, TXvec4 normal2,
                                       TXvec4 mvPos0,  TXvec4 mvPos1,  TXvec4 mvPos2,
                                       float interpolatedZ)
{
    TXvec4 interpolatedNormals   = TX_VEC4_ZERO;
    TXvec4 interpolatedPositions = TX_VEC4_W1;

    // TXvec4 interpolatedTexCoords = TX_VEC4_ZERO;
    // Texture-coordinate interpolation
    // is not implemented yet

    TXvec3 viewDir;
    switch (vertexInfo) {
        case TX_POSITION:
            txVec4Copy(outputColor, txGetColorPtr());
            break;
        case TX_POSITION_COLOR:
            txInterpolateVertexElement(outputColor,
                                       tri->v0_attr0, tri->v1_attr0, tri->v2_attr0,
                                       weights,
                                       zValues,
                                       interpolatedZ);
            break;
        case TX_POSITION_NORMAL:
            switch (shadeModel) {
                case TX_UNLIT:
                    txVec4Copy(outputColor, txGetColorPtr());
                    return;
                case TX_FLAT:
                    txVec3Zero(outputColor);
                    txAverageVertexElement(interpolatedNormals,
                                           normal0, normal1, normal2);

                    txAverageVertexElement(interpolatedPositions,
                                           mvPos0, mvPos1, mvPos2);
                    break;
                case TX_SMOOTH:
                    txVec3Zero(outputColor);
                    txInterpolateVertexElement(interpolatedNormals,
                                               normal0, normal1, normal2,
                                               weights,
                                               zValues,
                                               interpolatedZ);

                    txInterpolateVertexElement(interpolatedPositions,
                                               mvPos0, mvPos1, mvPos2,
                                               weights,
                                               zValues,
                                               interpolatedZ);
                    break;
            }

            txVec3Negate(viewDir, interpolatedPositions);
            txVec3Normalize(viewDir, viewDir);
            txVec3Normalize(interpolatedNormals, interpolatedNormals);

            txComputeDirLight(outputColor,
                              interpolatedNormals,
                              viewDir);
            txComputePointLight(outputColor,
                                interpolatedNormals,
                                interpolatedPositions,
                                viewDir);
            txComputeSpotLight(outputColor,
                               interpolatedNormals,
                               interpolatedPositions,
                               viewDir);
            break;
        case TX_POSITION_TEXCOORD:
            txOutputMessage(TX_INFO, "[CursedGL] runFragmentShader: texture interpolation is currently not implemented");
            break;
        case TX_POSITION_COLOR_NORMAL:
            txInterpolateVertexElement(outputColor,
                                       tri->v0_attr0, tri->v1_attr0, tri->v2_attr0,
                                       weights,
                                       zValues,
                                       interpolatedZ);

            switch (shadeModel) {
                case TX_UNLIT:
                    return;
                case TX_FLAT:
                    txAverageVertexElement(interpolatedNormals,
                                           normal0, normal1, normal2);

                    txAverageVertexElement(interpolatedPositions,
                                           mvPos0, mvPos1, mvPos2);
                    break;
                case TX_SMOOTH:
                    txInterpolateVertexElement(interpolatedNormals,
                                               normal0, normal1, normal2,
                                               weights,
                                               zValues,
                                               interpolatedZ);

                    txInterpolateVertexElement(interpolatedPositions,
                                               mvPos0, mvPos1, mvPos2,
                                               weights,
                                               zValues,
                                               interpolatedZ);
                    break;
            }

            txVec3Negate(viewDir, interpolatedPositions);
            txVec3Normalize(viewDir, viewDir);
            txVec3Normalize(interpolatedNormals, interpolatedNormals);

            txComputeDirLight(outputColor,
                              interpolatedNormals,
                              viewDir);
            txComputePointLight(outputColor,
                                interpolatedNormals,
                                interpolatedPositions,
                                viewDir);
            txComputeSpotLight(outputColor,
                               interpolatedNormals,
                               interpolatedPositions,
                               viewDir);
            break;
        case TX_POSITION_COLOR_TEXCOORD:
            txOutputMessage(TX_INFO, "[CursedGL] runFragmentShader: texture interpolation is currently not implemented");
            break;
        case TX_POSITION_NORMAL_TEXCOORD:
            txOutputMessage(TX_INFO, "[CursedGL] runFragmentShader: texture interpolation is currently not implemented");
            break;
        case TX_POSITION_COLOR_NORMAL_TEXCOORD:
            txOutputMessage(TX_INFO, "[CursedGL] runFragmentShader: texture interpolation is currently not implemented");
            break;
    }
}

////////////////////////////////////////
bool txShouldCullFace(TXvec3 view_v0,
                      TXvec3 view_v1,
                      TXvec3 view_v2)
{
    if (txIsCullingEnabled()) {
        if (cullFace == TX_FRONT_AND_BACK)
            return true;
        else if (cullFace == TX_NONE)
            return false;

        TXvec3 vec0, vec1;
        txVec3Sub(vec0, view_v0, view_v1);
        txVec3Sub(vec1, view_v0, view_v2);

        txVec3Normalize(vec0, vec0);
        txVec3Normalize(vec1, vec1);

        TXvec3 normal;
        txVec3Cross(normal, vec0, vec1);

        TXvec3 normalPos = TX_VEC3_ZERO;
        txVec3Add(normalPos, normalPos, view_v0);
        txVec3Add(normalPos, normalPos, view_v1);
        txVec3Add(normalPos, normalPos, view_v2);
        txVec3ScalarDiv(normalPos, normalPos, 3.0f);

        TXvec3 normalToEye;
        txVec3Negate(normalToEye, normalPos);
        txVec3Normalize(normalToEye, normalToEye);

        float d = txVec3Dot(normal, normalToEye);

        if ((windOrder == TX_CCW && cullFace == TX_BACK) ||
            (windOrder == TX_CW  && cullFace == TX_FRONT))
            return d < 0.0f;
        else
            return d > 0.0f;
    }
    return false;
}

////////////////////////////////////////
bool txIsPointInLine(int i,
                     int j,
                     TXvec3 v0,
                     TXvec3 v1)
{
    float s_num = v1[1] - v0[1];
    float s_den = v1[0] - v0[0];
    if (txFloatEquals(s_den, 0.0f) || txFloatEquals(s_num, 0.0f))
        return true;

    float s = s_num / s_den;
    float s_inv = -1.0f / s;
    float x_num = v0[0] * s - (float)i * s_inv - v0[1] + (float)j;
    float x_den = s - s_inv;
    float x = x_num / x_den;
    float y = x * s + v0[1] - v0[0] * s;
    if (txFloatWithin(x, (float)i, TX_LINE_BIAS) && txFloatWithin(y, (float)j, TX_LINE_BIAS))
        return true;
    return false;
}

////////////////////////////////////////
bool txIsPointInTriangle(int i,
                         int j,
                         TXvec3 v0,
                         TXvec3 v1,
                         TXvec3 v2,
                         TXvec3 weights)
{
    float w0_num = v0[0] * (v2[1] - v0[1]) + ((float)j - v0[1]) * (v2[0] - v0[0]) - (float)i * (v2[1] - v0[1]);
    float w0_den = (v1[1] - v0[1]) * (v2[0] - v0[0]) - (v1[0] - v0[0]) * (v2[1] - v0[1]);
    if (txFloatEquals(w0_den, 0.0f)) {
        weights[0] = 0.0f;
        weights[1] = 0.0f;
        weights[2] = 1.0f;
        return true;
    }
    float w0 = w0_num / w0_den;
    float w1_num = (float)j - v0[1] - w0 * (v1[1] - v0[1]);
    float w1_den = v2[1] - v0[1];
    if (txFloatEquals(w1_den, 0.0f)) {
        weights[0] = 0.0f;
        weights[1] = 1.0f;
        weights[2] = 0.0f;
        return true;
    }
    float w1 = w1_num / w1_den;
    if (w0 >= 0.0f && w1 >= 0.0f && (w0 + w1) <= 1.0f) {
        weights[0] = 1.0f - w0 - w1;
        weights[1] = w0;
        weights[2] = w1;
        return true;
    }
    return false;
}

////////////////////////////////////////
void txDrawPoint(TXvec4 v0)
{
    TXvec4 viewport_v0;
    txConvertToViewSpace(viewport_v0, v0);
    txConvertToClipSpace(viewport_v0, viewport_v0);
    txConvertToWindowSpace(viewport_v0, viewport_v0);

    int x = (int)viewport_v0[0];
    int y = (int)viewport_v0[1];

    int fbWidth  = txGetFramebufferWidth();
    int fbHeight = txGetFramebufferHeight();

    float depth = viewport_v0[2];
    if ((x < 0 || x >= fbWidth) ||
        (y < 0 || y >= fbHeight) ||
        (depth < 0.0f || depth > 1.0f))
    {
        return;
    }

    // Points currently do not react to lighting.
    // Not sure if they should

    TXpixel_t* p = txGetPixelFromBackFramebuffer(y, x);
    if (txIsDepthTestEnabled() && txCompareDepth(depth, p->depth)) {
        txVec4Copy(p->color, rasterColor);
        if (txGetDepthMask())
            p->depth = depth;
    }
    else
        txVec4Copy(p->color, rasterColor);
}

////////////////////////////////////////
void txDrawLine(TXvec4 v0, TXvec4 v1)
{
    TXvec4 viewport_v0, viewport_v1;
    txConvertToViewSpace(viewport_v0, v0);
    txConvertToViewSpace(viewport_v1, v1);

    txConvertToClipSpace(viewport_v0, viewport_v0);
    txConvertToClipSpace(viewport_v1, viewport_v1);

    txConvertToWindowSpace(viewport_v0, viewport_v0);
    txConvertToWindowSpace(viewport_v1, viewport_v1);

    int fbWidth  = txGetFramebufferWidth();
    int fbHeight = txGetFramebufferHeight();

    int minx = (int)fmaxf(0.0f, fminf(viewport_v0[0], viewport_v1[0]));
    int miny = (int)fmaxf(0.0f, fminf(viewport_v0[1], viewport_v1[1]));

    int maxx = (int)fminf((float)fbWidth  - TX_FB_BIAS, fmaxf(viewport_v0[0],
                                                              viewport_v1[0]));
    int maxy = (int)fminf((float)fbHeight - TX_FB_BIAS, fmaxf(viewport_v0[1],
                                                              viewport_v1[1]));

    if (minx < 0 || miny < 0 || maxx >= fbWidth || maxy >= fbHeight)
        return;

    // Lines also do not react to light sources. Not sure if they should.

    for (int i = miny; i <= maxy; ++i) {
        for (int j = minx; j <= maxx; ++j) {
            if (txIsPointInLine(j, i, viewport_v0, viewport_v1)) {
                float interpolatedDepth = txInterpolateLineDepth(j,
                                                                 i,
                                                                 viewport_v0,
                                                                 viewport_v1);

                // Perhaps instead of clipping the line,
                // we could simply reject those pixels on
                // the line that fall outside of near and far
                // planes?
                if (interpolatedDepth < 0.0f || interpolatedDepth > 1.0f)
                    continue;

                TXpixel_t* p = txGetPixelFromBackFramebuffer(i, j);
                if (txIsDepthTestEnabled()) {
                    if (txCompareDepth(interpolatedDepth, p->depth)) {
                        txVec4Copy(p->color, rasterColor);
                        if (txGetDepthMask())
                            p->depth = interpolatedDepth;
                    }
                }
                else
                    txVec4Copy(p->color, rasterColor);
            }
        }
    }
}

////////////////////////////////////////
static int clipVertices(TXtriangle_t triangles[])
{
    static TXvec3 nearPlaneNormal = { 0.0f, 0.0f,  1.0f };
    static TXvec3 farPlaneNormal  = { 0.0f, 0.0f, -1.0f };

    float* projectionMatrix = txGetProjectionMatrix();

    float near = projectionMatrix[14] / (projectionMatrix[10] - 1.0f);
    float far = projectionMatrix[14] / (projectionMatrix[10] + 1.0f);

    TXvec3 nearPlanePosition = { 0.0f, 0.0f, near };
    TXvec3 farPlanePosition  = { 0.0f, 0.0f, far  };

    // first check against the near plane
    int numTriangles = txClipTriangleAgainstPlane(&triangles[0],
                                                  &triangles[1],
                                                  nearPlanePosition,
                                                  nearPlaneNormal,
                                                  &triangles[0]);

    // then check against the far plane
    int totalNumTriangles;
    switch (numTriangles) {
        case 0:
            return 0;
        case 1:
            return txClipTriangleAgainstPlane(&triangles[0],
                                              &triangles[1],
                                              farPlanePosition,
                                              farPlaneNormal,
                                              &triangles[0]);
        case 2:
            totalNumTriangles = numTriangles;
            numTriangles = txClipTriangleAgainstPlane(&triangles[0],
                                                      &triangles[totalNumTriangles],
                                                      farPlanePosition,
                                                      farPlaneNormal,
                                                      &triangles[0]);
            if (numTriangles == 0)
                return 0;
            else if (numTriangles > 1)
                ++totalNumTriangles;

            numTriangles = txClipTriangleAgainstPlane(&triangles[1],
                                                      &triangles[totalNumTriangles],
                                                      farPlanePosition,
                                                      farPlaneNormal,
                                                      &triangles[1]);
            if (numTriangles == 0)
                return 0;
            else if (numTriangles > 1)
                ++totalNumTriangles;
            return totalNumTriangles;
    }
    return 0;
}

////////////////////////////////////////
static void renderTriangles(enum TXvertexInfo vertexInfo,
                            TXtriangle_t triangles[],
                            int numTriangles)
{
    for (int tri = 0; tri < numTriangles; ++tri) {
        TXvec4 viewport_v0, viewport_v1, viewport_v2;

        TXvec4 normal0, normal1, normal2;
        txVec4Zero(normal0);
        txVec4Zero(normal1);
        txVec4Zero(normal2);

        TXvec4 mvPos0, mvPos1, mvPos2;
        txVec4Zero(mvPos0);
        txVec4Zero(mvPos1);
        txVec4Zero(mvPos2);

        TXvec3 zValues;

        ////////////////////////////////////////
        /////// VERTEX SHADER EMULATION ////////
        ////////////////////////////////////////

        runVertexShader(vertexInfo,
                        &triangles[tri],
                        viewport_v0, viewport_v1, viewport_v2,
                        zValues,
                        normal0, normal1, normal2,
                        mvPos0, mvPos1, mvPos2);

        ////////////////////////////////////////
        //////// VERTEX SHADER COMPLETE ////////
        ////////////////////////////////////////

        int fbWidth  = txGetFramebufferWidth();
        int fbHeight = txGetFramebufferHeight();

        int minx = (int)fmaxf(0.0f, txMin3(viewport_v0[0],
                                           viewport_v1[0],
                                           viewport_v2[0]));
        int miny = (int)fmaxf(0.0f, txMin3(viewport_v0[1],
                                           viewport_v1[1],
                                           viewport_v2[1]));
        int maxx = (int)fminf((float)fbWidth  - TX_FB_BIAS, txMax3(viewport_v0[0],
                                                                   viewport_v1[0],
                                                                   viewport_v2[0]));
        int maxy = (int)fminf((float)fbHeight - TX_FB_BIAS, txMax3(viewport_v0[1],
                                                                   viewport_v1[1],
                                                                   viewport_v2[1]));

        ////////////////////////////////////////
        /////// BARYCENTRIC COORDINATES ////////
        ////////////////////////////////////////
        TXvec3 weights;

        ////////////////////////////////////////
        /////// OUTPUT COLOR OF THE PIXEL //////
        ////////////////////////////////////////
        TXvec4 outputColor = TX_VEC4_W1;

        for (int i = miny; i <= maxy; ++i) {
            for (int j = minx; j <= maxx; ++j) {
                if (txIsPointInTriangle(j,
                                        i,
                                        viewport_v0,
                                        viewport_v1,
                                        viewport_v2,
                                        weights)) {
                    float interpolatedDepth = txVec3Dot(zValues, weights);

                    TXpixel_t* p = txGetPixelFromBackFramebuffer(i, j);
                    if (txIsDepthTestEnabled()) {
                        if (txCompareDepth(interpolatedDepth, p->depth)) {

                            ////////////////////////////////////////
                            /////// FRAGMENT SHADER EMULATION //////
                            ////////////////////////////////////////
                            runFragmentShader(vertexInfo,
                                              &triangles[tri],
                                              outputColor,
                                              weights,
                                              zValues,
                                              normal0, normal1, normal2,
                                              mvPos0, mvPos1, mvPos2,
                                              interpolatedDepth);
                            ////////////////////////////////////////
                            /////// FRAGMENT SHADER COMPLETE ///////
                            ////////////////////////////////////////

                            txVec4Clamp(outputColor, outputColor, 0.0f, 1.0f);
                            txVec4Copy(p->color, outputColor);
                            if (txGetDepthMask())
                                p->depth = interpolatedDepth;
                        }
                    }
                    else {
                        ////////////////////////////////////////
                        /////// FRAGMENT SHADER EMULATION //////
                        ////////////////////////////////////////
                        runFragmentShader(vertexInfo,
                                          &triangles[tri],
                                          outputColor,
                                          weights,
                                          zValues,
                                          normal0, normal1, normal2,
                                          mvPos0, mvPos1, mvPos2,
                                          interpolatedDepth);
                        ////////////////////////////////////////
                        /////// FRAGMENT SHADER COMPLETE ///////
                        ////////////////////////////////////////

                        txVec4Clamp(outputColor, outputColor, 0.0f, 1.0f);
                        txVec4Copy(p->color, outputColor);
                    }
                }
            }
        }
    }
}

////////////////////////////////////////
void txDrawTriangle(TXvec4 v0[],
                    TXvec4 v1[],
                    TXvec4 v2[],
                    enum TXvertexInfo vertexInfo)
{
    TXvec4 pos_v0, pos_v1, pos_v2;

    // Face-culling occurs in view-space
    txConvertToViewSpace(pos_v0, v0[0]);
    txConvertToViewSpace(pos_v1, v1[0]);
    txConvertToViewSpace(pos_v2, v2[0]);

    if (txShouldCullFace(pos_v0, pos_v1, pos_v2))
        return;

    // We may have to clip our triangle
    txConvertToClipSpace(pos_v0, pos_v0);
    txConvertToClipSpace(pos_v1, pos_v1);
    txConvertToClipSpace(pos_v2, pos_v2);

    // Allocate enough memory for max
    // possible number of triangles
    TXtriangle_t triangles[4];

    // Clip coordinates for clipping
    txVec4Copy(triangles[0].v0_pos, pos_v0);
    txVec4Copy(triangles[0].v1_pos, pos_v1);
    txVec4Copy(triangles[0].v2_pos, pos_v2);

    // World coordinates for lighting
    txVec4Copy(triangles[0].v0_obj_pos, v0[0]);
    txVec4Copy(triangles[0].v1_obj_pos, v1[0]);
    txVec4Copy(triangles[0].v2_obj_pos, v2[0]);

    // Populate the triangles[0] by copying the
    // appropriate attributes based on a given
    // VAO configuration
    switch (vertexInfo) {
        case TX_POSITION_COLOR_NORMAL_TEXCOORD:
            txVec4Copy(triangles[0].v0_attr0, v0[1]);
            txVec4Copy(triangles[0].v1_attr0, v1[1]);
            txVec4Copy(triangles[0].v2_attr0, v2[1]);

            txVec4Copy(triangles[0].v0_attr1, v0[2]);
            txVec4Copy(triangles[0].v1_attr1, v1[2]);
            txVec4Copy(triangles[0].v2_attr1, v2[2]);

            txVec4Copy(triangles[0].v0_attr2, v0[3]);
            txVec4Copy(triangles[0].v1_attr2, v1[3]);
            txVec4Copy(triangles[0].v2_attr2, v2[3]);
            break;
        case TX_POSITION_COLOR_NORMAL:
        case TX_POSITION_COLOR_TEXCOORD:
        case TX_POSITION_NORMAL_TEXCOORD:
            txVec4Copy(triangles[0].v0_attr0, v0[1]);
            txVec4Copy(triangles[0].v1_attr0, v1[1]);
            txVec4Copy(triangles[0].v2_attr0, v2[1]);

            txVec4Copy(triangles[0].v0_attr1, v0[2]);
            txVec4Copy(triangles[0].v1_attr1, v1[2]);
            txVec4Copy(triangles[0].v2_attr1, v2[2]);
            break;
        case TX_POSITION_COLOR:
        case TX_POSITION_NORMAL:
        case TX_POSITION_TEXCOORD:
            txVec4Copy(triangles[0].v0_attr0, v0[1]);
            txVec4Copy(triangles[0].v1_attr0, v1[1]);
            txVec4Copy(triangles[0].v2_attr0, v2[1]);
            break;
        case TX_POSITION:
            break;
    }

    int numTriangles = clipVertices(triangles);
    renderTriangles(vertexInfo, triangles, numTriangles);
}
