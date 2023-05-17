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
#include "init.h"

#include <notcurses/notcurses.h>

#include <stdint.h>

////////////////////////////////////////
#define TX_COLOR_BIT (1ULL << 1)
#define TX_DEPTH_BIT (1ULL << 2)
#define TX_DEPTH_TEST (1ULL << 3)
#define TX_CULL_FACE  (1ULL << 4)

////////////////////////////////////////
enum TXdepthFunc { TX_LESS,
                   TX_LEQUAL,
                   TX_EQUAL,
                   TX_GEQUAL,
                   TX_GREATER,
                   TX_NOTEQUAL };

////////////////////////////////////////
struct TXframebufferInfo
{
    TXpixel_t* framebuffers[2];
    int currentFramebuffer;

    int width;
    int height;

    TXvec4 clearColor;

    unsigned flags;

    enum TXdepthFunc depthFunc;
    bool depthMask;
    float depthClear;

    struct ncplane* renderPlane;
    struct ncvisual_options options;

    uint32_t* raw_framebuffer;
};
typedef struct TXframebufferInfo TXframebufferInfo_t;

////////////////////////////////////////
bool txCompareDepth(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, float interpolatedDepth, float pixelDepth);

////////////////////////////////////////
float txGetFramebufferAspectRatio(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo);

////////////////////////////////////////
void txGetEffectiveDims(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, int* effectiveWidth, int* effectiveHeight);

////////////////////////////////////////
bool txViewport(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, int width, int height);

////////////////////////////////////////
void txClear(const TXframebufferInfo_t* framebufferInfo);

////////////////////////////////////////
TXpixel_t* txGetPixelFromCurrentFramebuffer(const TXframebufferInfo_t* framebufferInfo, int row, int col);

////////////////////////////////////////
TXpixel_t* txGetPixelFromDisplayFramebuffer(const TXframebufferInfo_t* framebufferInfo, int row, int col);

////////////////////////////////////////
bool txSetPixelInCurrentFramebuffer(TXframebufferInfo_t* framebufferInfo, int row, int col, const TXpixel_t* pixel)

////////////////////////////////////////
bool txSetPixelInDisplayFramebuffer(TXframebufferInfo_t* framebufferInfo, int row, int col, const TXpixel_t* pixel)

////////////////////////////////////////
void txDrawFramebuffer(TXappInfo_t* appInfo, TXframebufferInfo_t* framebufferInfo, int offsetX, int offsetY, int limitX, int limitY);

////////////////////////////////////////
void txFreeFramebuffer(TXframebufferInfo_t* framebufferInfo);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
