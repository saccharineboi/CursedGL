// Copyright (C) 2022 saccharineboi

#include "framebuffer.h"
#include "pixel.h"
#include "init.h"
#include "error.h"

#include <stdlib.h>
#include <notcurses/notcurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

////////////////////////////////////////
bool txCompareDepth(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, float interpolatedDepth, float pixelDepth)
{
    switch (framebufferInfo->depthFunc) {
        case TX_LESS:
            return interpolatedDepth < pixelDepth;
        case TX_LEQUAL:
            return interpolatedDepth <= pixelDepth;
        case TX_EQUAL:
            return txFloatEquals(interpolatedDepth, pixelDepth);
        case TX_GEQUAL:
            return interpolatedDepth >= pixelDepth;
        case TX_GREATER:
            return interpolatedDepth > pixelDepth;
        case TX_NOTEQUAL:
            return !txFloatEquals(interpolatedDepth, pixelDepth);
        default:
            if (appInfo->messageCallback) {
                appInfo->messageCallback(TX_WARNING, "[CursedGL] txCompareDepth: invalid value");
            }
            return false;
    }
}

////////////////////////////////////////
float txGetFramebufferAspectRatio(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo)
{
    int effectiveWidth, effectiveHeight;
    txGetEffectiveDims(appInfo, framebufferInfo, &effectiveWidth, &effectiveHeight);
    return (float)effectiveWidth / (float)effectiveHeight;
}

////////////////////////////////////////
void txGetEffectiveDims(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, int* effectiveWidth, int* effectiveHeight)
{
    switch (appInfo->blitter) {
        case NCBLIT_1x1:
            *effectiveWidth = framebufferInfo->width;
            *effectiveHeight = framebufferInfo->height;
            break;
        case NCBLIT_2x1:
            *effectiveWidth = framebufferInfo->width;
            *effectiveHeight = framebufferInfo->height * 2;
            break;
        case NCBLIT_2x2:
            *effectiveWidth = framebufferInfo->width * 2;
            *effectiveHeight = framebufferInfo->height * 2;
            break;
        case NCBLIT_3x2:
            *effectiveWidth = framebufferInfo->width * 2;
            *effectiveHeight = framebufferInfo->height * 3;
            break;
        case NCBLIT_BRAILLE:
            *effectiveWidth = framebufferInfo->width * 2;
            *effectiveHeight = framebufferInfo->height * 4;
            break;
        case NCBLIT_4x1:
            *effectiveWidth = framebufferInfo->width;
            *effectiveHeight = framebufferInfo->height * 4;
            break;
        case NCBLIT_8x1:
            *effectiveWidth = framebufferInfo->width;
            *effectiveHeight = framebufferInfo->height * 8;
            break;
        case NCBLIT_PIXEL:
            *effectiveWidth = 0;
            *effectiveHeight = 0;
            break;
        case NCBLIT_DEFAULT:
            *effectiveWidth = framebufferInfo->width;
            *effectiveHeight = framebufferInfo->height;
            break;
    }
}

////////////////////////////////////////
void txViewport(int width, int height)
{
    ncblitter_e currentBlitter = txGetCurrentBlitter();
    int actualWidth = 0, actualHeight = 0;
    getActualDims(currentBlitter, &actualWidth,
                                  &actualHeight,
                                  framebufferWidth,
                                  framebufferHeight);

    if (actualWidth   != width   ||
        actualHeight  != height) {

        getEffectiveDims(currentBlitter, &newFramebufferWidth,
                                         &newFramebufferHeight,
                                         width,
                                         height);

        viewportResized         = true;

        txOutputMessage(TX_INFO, "[CursedGL] txViewPort: actual dims: %d:%d, param dims: %d:%d, effective dims: %d:%d", actualWidth, actualHeight, width, height, newFramebufferWidth, newFramebufferHeight);
    }
}

////////////////////////////////////////
void txClear(int flags)
{
    for (int i = 0; i < framebufferWidth * framebufferHeight; ++i) {
        if (flags & TX_COLOR_BIT)
            txVec4Copy(framebuffers[BACK_BUFFER][i].color, framebufferClearColor);
        if ((stateFlags & TX_DEPTH_TEST) && (flags & TX_DEPTH_BIT))
            framebuffers[BACK_BUFFER][i].depth = depthClear;
    }
}

////////////////////////////////////////
TXpixel_t* txGetPixelFromFramebuffer(int row, int col, enum TXframebufferType type)
{
    int pos = row * framebufferWidth + col;
    if (pos < 0 || pos > framebufferWidth * framebufferHeight) {
        txOutputMessage(TX_ERROR, "[CursedGL] txGetPixelFromFramebuffer: (row: %d, col: %d) is invalid position for pixel", row, col);
        return NULL;
    }
    return &framebuffers[type][pos];
}

////////////////////////////////////////
void txSetPixelInFramebuffer(int row, int col, TXpixel_t* p, enum TXframebufferType type)
{
    int pos = row * framebufferWidth + col;
    if (pos >= 0 && pos <= framebufferWidth * framebufferHeight) {
        txVec4Copy(framebuffers[type][pos].color, p->color);
    } else {
        txOutputMessage(TX_ERROR, "[CursedGL] txSetPixelInFramebuffer: (row: %d, col: %d) is invalid position for pixel", row, col);
    }
}

////////////////////////////////////////
/// This function is running continuously
/// on a different thread, and it will
/// continue to run until the swap thread
/// (aka the main thread) tells it to stop.
///
/// This is where the contents of the framebuffer
/// is given to notcurses to be rendered to the
/// terminal
////////////////////////////////////////
static void* drawFramebuffer()
{
    while (true) {
        if (stopRendering)
            return NULL;

        if (!startRendering) {
            usleep(renderThreadWait);
            continue;
        }

        ncblitter_e currentBlitter = txGetCurrentBlitter();

        struct ncvisual_options options = {
            .n          = renderPlane,
            .scaling    = NCSCALE_NONE,
            .leny       = (unsigned)framebufferHeight,
            .lenx       = (unsigned)framebufferWidth,
            .blitter    = currentBlitter
        };

        currentlyRendering = true;
        for (int i = 0; i < framebufferHeight; ++i) {
            for (int j = 0; j < framebufferWidth; ++j) {
                TXpixel_t* currentPixel = txGetPixelFromFrontFramebuffer(i, j);
                uint32_t u_r = (uint32_t)(currentPixel->color[0] * 255.0f);
                uint32_t u_g = (uint32_t)(currentPixel->color[1] * 255.0f);
                uint32_t u_b = (uint32_t)(currentPixel->color[2] * 255.0f);

                raw_framebuffer[i * framebufferWidth + j] = (((uint32_t)255 << 24) |
                                                                       (u_b << 16) |
                                                                       (u_g << 8)  |
                                                                       (u_r << 0));
            }
        }
        if (!viewportResized) {
            ncblit_rgba(raw_framebuffer,
                        txGetFramebufferWidth() * (int)(sizeof(uint32_t)),
                        &options);
            notcurses_render(txGetContext());
        }

        startRendering = false;
        currentlyRendering = false;
    }
}
