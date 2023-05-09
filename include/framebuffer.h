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
};
typedef struct TXframebufferInfo TXframebufferInfo_t;

////////////////////////////////////////
bool txCompareDepth(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, float interpolatedDepth, float pixelDepth);

////////////////////////////////////////
float txGetFramebufferAspectRatio(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo);

////////////////////////////////////////
void txGetEffectiveDims(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, int* effectiveWidth, int* effectiveHeight);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
