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
/// Sets the depth compare function to be
/// used during depth-testing
////////////////////////////////////////
void txDepthFunc(enum TXdepthFunc depthFunc);

////////////////////////////////////////
enum TXdepthFunc txGetDepthFunc();

////////////////////////////////////////
/// Compares interpolatedDepth with pixelDepth
/// as specified by the depth compare function
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
/// Returns the aspect ratio of the
/// framebuffer, i.e. framebuffer width
/// divided by framebuffer height
////////////////////////////////////////
float txGetFramebufferAspectRatio();

////////////////////////////////////////
/// Returns the amount of time the swap
/// thread sleeps before resuming again.
/// Swap thread, while sleeping, blocks
/// the main thread (as of current
/// implementation, swap thread is just
/// an alias for main thread)
////////////////////////////////////////
unsigned txGetSwapThreadWait();

////////////////////////////////////////
/// Returns the amount of time the swap
/// thread sleeps before the render thread
/// is finished. Swap thread, while sleeping,
/// blocks the main thread
////////////////////////////////////////
unsigned txGetRenderThreadWait();

////////////////////////////////////////
/// Returns whether the render thread is
/// busy rendering to the screen
////////////////////////////////////////
bool txIsCurrentlyRendering();

////////////////////////////////////////
/// Defines the dimensions of a viewport
/// for CursedGL to render into
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
/// Sets the color the framebuffer will
/// be cleared with during txClear(COLOR_BIT)
/// call
////////////////////////////////////////
void txClearColor4f(float r, float g, float b, float a);

////////////////////////////////////////
#define txClearColor3f(r, g, b) txClearColor4f(r, g, b, 1.0f)

////////////////////////////////////////
/// Sets the color the framebuffer will
/// be cleared with during txClear(COLOR_BIT)
/// call but with a vector
////////////////////////////////////////
void txClearColor4fv(TXvec4 color);

////////////////////////////////////////
/// Clears the framebuffer according to
/// the flags parameter
////////////////////////////////////////
void txClear(int flags);

////////////////////////////////////////
/// Returns a single pixel object from
/// the (type) framebuffer where type is
/// either TX_FRAMEBUFFER_FRONT or
/// TX_FRAMEBUFFER_BACK at position (row, col)
////////////////////////////////////////
TXpixel_t* txGetPixelFromFramebuffer(int row, int col, enum TXframebufferType type);

////////////////////////////////////////
#define txGetPixelFromFrontFramebuffer(row, col) txGetPixelFromFramebuffer(row, col, txGetFrontBuffer())

////////////////////////////////////////
#define txGetPixelFromBackFramebuffer(row, col) txGetPixelFromFramebuffer(row, col, txGetBackBuffer())

////////////////////////////////////////
/// Sets a single pixel in the (type)
/// framebuffer where type is either
/// TX_FRAMEBUFFER_FRONT or TX_FRAMEBUFFER_BACK
/// at position (row, col)
////////////////////////////////////////
void txSetPixelInFramebuffer(int row, int col, TXpixel_t* p, enum TXframebufferType type);

////////////////////////////////////////
/// Alias for txSetPixelInFramebuffer
/// where type is TX_FRAMEBUFFER_BACK
////////////////////////////////////////
#define txSetPixelInBackFramebuffer(row, col, pixel) txSetPixelInFramebuffer(row, col, pixel, txGetBackBuffer())

////////////////////////////////////////
/// Alias for txSetPixelInFramebuffer
/// where type is TX_FRAMEBUFFER_FRONT
////////////////////////////////////////
#define txSetPixelInFrontFramebuffer(row, col, pixel) txSetPixelInFramebuffer(row, col, pixel, txGetFrontBuffer())

////////////////////////////////////////
/// Frees the framebuffers and stops the
/// render thread. Returns true on success,
/// false otherwise
////////////////////////////////////////
bool txFreeFramebuffer();

////////////////////////////////////////
void txSwapBuffers();

////////////////////////////////////////
enum TXframebufferType txGetFrontBuffer();

////////////////////////////////////////
enum TXframebufferType txGetBackBuffer();

////////////////////////////////////////
/// Sets the amount of time the swap
/// thread sleeps before resuming again.
/// Sleep time is specified in milliseconds.
/// This function implicitly sets swapThreadWait
/// and renderThreadWait
////////////////////////////////////////
void txSetWaitMilliseconds(unsigned milliseconds);

////////////////////////////////////////
/// Returns the amount of time the swap
/// thread sleeps before resuming again.
/// Sleep time is specified in milliseconds
////////////////////////////////////////
unsigned txGetWaitMilliseconds();

////////////////////////////////////////
/// Swap thread sleep time to render thread
/// sleep time ratio is used to regulate the amount
/// of time render thread has to wait for swap
/// thread to complete one swap (i.e. run the game
/// loop once)
////////////////////////////////////////
void txSetSwapToRenderRatio(unsigned ratio);

////////////////////////////////////////
/// If the depth mask is false, then the
/// depth values are read-only, otherwise
/// depth values are both readable and
/// writable
////////////////////////////////////////
void txDepthMask(bool flag);

////////////////////////////////////////
/// Sets the depth value the depth buffer bits
/// will be cleared with during txClear
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
