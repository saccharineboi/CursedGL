// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "vec.h"
#include "pixel.h"
#include "common.h"

#include <notcurses/notcurses.h>

////////////////////////////////////////
enum TXframebufferType { TX_FRAMEBUFFER_FRONT, TX_FRAMEBUFFER_BACK };

////////////////////////////////////////
/// These constants are passed to txClear
////////////////////////////////////////
#define TX_COLOR_BIT (1ULL << 1)
#define TX_DEPTH_BIT (1ULL << 2)

////////////////////////////////////////
/// These constants are passed to tx{Enable,Disable}
////////////////////////////////////////
#define TX_DEPTH_TEST (1ULL << 1)
#define TX_CULL_FACE  (1ULL << 2)

////////////////////////////////////////
/// These constants are passed to txDepthFunc
////////////////////////////////////////
enum TXdepthFunc { TX_LESS,
                   TX_LEQUAL,
                   TX_EQUAL,
                   TX_GEQUAL,
                   TX_GREATER,
                   TX_NOTEQUAL };

////////////////////////////////////////
void txDepthFunc(enum TXdepthFunc depthFunc);

////////////////////////////////////////
enum TXdepthFunc txGetDepthFunc();

////////////////////////////////////////
bool txCompareDepth(float interpolatedDepth, float pixelDepth);

////////////////////////////////////////
bool txIsDepthTestEnabled();

////////////////////////////////////////
bool txIsCullingEnabled();

////////////////////////////////////////
void txEnable(unsigned flags);

////////////////////////////////////////
void txDisable(unsigned flags);

////////////////////////////////////////
bool txInitFramebuffer(struct ncplane* renderPlane);

////////////////////////////////////////
int txGetFramebufferWidth();

////////////////////////////////////////
int txGetFramebufferHeight();

////////////////////////////////////////
void txGetFramebufferMaxDims(int* width, int* height);

////////////////////////////////////////
float txGetFramebufferAspectRatio();

////////////////////////////////////////
unsigned txGetSwapThreadWait();

////////////////////////////////////////
unsigned txGetRenderThreadWait();

////////////////////////////////////////
bool txIsCurrentlyRendering();

////////////////////////////////////////
void txViewport(int width, int height);

////////////////////////////////////////
/// Alias for txViewport(WINDOW_WIDTH, WINDOW_HEIGHT)
/// where WINDOW_WIDTH is the width of the terminal, and
/// WINDOW_HEIGHT is the height of the terminal
////////////////////////////////////////
void txViewportMax();

////////////////////////////////////////
void txGetFramebufferDims(int* width, int* height);

////////////////////////////////////////
/// Returns the size of one of the framebuffers
/// in bytes. Note that both framebuffers
/// (front and back) are of the same size
////////////////////////////////////////
size_t txGetFramebufferSize();

////////////////////////////////////////
/// Returns the size of one of the raw
/// framebuffers in bytes. Note that both
/// framebuffers (front and back) are of
/// the same size
////////////////////////////////////////
size_t txGetRawFramebufferSize();

////////////////////////////////////////
void txClearColor4f(float r, float g, float b, float a);

////////////////////////////////////////
#define txClearColor3f(r, g, b) txClearColor4f(r, g, b, 1.0f)

////////////////////////////////////////
void txClearColor4fv(TXvec4 color);

////////////////////////////////////////
void txClear(int flags);

////////////////////////////////////////
TXpixel_t* txGetPixelFromFramebuffer(int row, int col, enum TXframebufferType type);

////////////////////////////////////////
#define txGetPixelFromFrontFramebuffer(row, col) txGetPixelFromFramebuffer(row, col, txGetFrontBuffer())

////////////////////////////////////////
#define txGetPixelFromBackFramebuffer(row, col) txGetPixelFromFramebuffer(row, col, txGetBackBuffer())

////////////////////////////////////////
void txSetPixelInFramebuffer(int row, int col, TXpixel_t* p, enum TXframebufferType type);

////////////////////////////////////////
#define txSetPixelInBackFramebuffer(row, col, pixel) txSetPixelInFramebuffer(row, col, pixel, txGetBackBuffer())

////////////////////////////////////////
#define txSetPixelInFrontFramebuffer(row, col, pixel) txSetPixelInFramebuffer(row, col, pixel, txGetFrontBuffer())

////////////////////////////////////////
bool txFreeFramebuffer();

////////////////////////////////////////
void txSwapBuffers();

////////////////////////////////////////
enum TXframebufferType txGetFrontBuffer();

////////////////////////////////////////
enum TXframebufferType txGetBackBuffer();

////////////////////////////////////////
void txSetWaitMilliseconds(unsigned milliseconds);

////////////////////////////////////////
unsigned txGetWaitMilliseconds();

////////////////////////////////////////
void txSetSwapToRenderRatio(unsigned ratio);

////////////////////////////////////////
void txDepthMask(bool flag);

////////////////////////////////////////
void txClearDepth(float value);

////////////////////////////////////////
float txGetClearDepth();

////////////////////////////////////////
bool txGetDepthMask();

////////////////////////////////////////
struct ncplane* txGetRenderPlane();

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
