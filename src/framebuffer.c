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
        default:
            *effectiveWidth = 0;
            *effectiveHeight = 0;
            break;
    }
}

////////////////////////////////////////
bool txViewport(const TXappInfo_t* appInfo, const TXframebufferInfo_t* framebufferInfo, int width, int height)
{
    if (!framebufferInfo->framebuffers[0] || !framebufferInfo->framebuffers[1] ||
        framebufferInfo->width != width || framebufferInfo->height != height) {
        framebufferInfo->width = width;
        framebufferInfo->height = height;

        int effectiveWidth, effectiveHeight;
        txGetEffectiveDims(appInfo, framebufferInfo, &effectiveWidth, &effectiveHeight);

        free(framebufferInfo->framebuffers[0]);
        free(framebufferInfo->framebuffers[1]);

        framebufferInfo->framebuffers[0] = (TXpixel_t*)malloc(effectiveWidth * effectiveHeight * sizeof(TXpixel_t));
        framebufferInfo->framebuffers[1] = (TXpixel_t*)malloc(effectiveWidth * effectiveHeight * sizeof(TXpixel_t));

        framebufferInfo->raw_framebuffer = (uint32_t*)malloc(effectiveWidth * effectiveHeight * sizeof(uint32_t));

        if (!framebufferInfo->framebuffers[0] || !framebufferInfo->framebuffers[1] || !framebufferInfo->raw_framebuffer) {
            return false;
        }

        framebufferInfo->currentFramebuffer = 0;
    }
}

////////////////////////////////////////
void txClear(const TXframebufferInfo_t* framebufferInfo)
{
    for (int i = 0; i < framebufferInfo->width * framebufferInfo->height; ++i) {
        if (framebufferInfo->flags & TX_COLOR_BIT)
            txVec4Copy(framebufferInfo->framebuffers[framebufferInfo->currentFramebuffer][i].color, framebufferInfo->clearColor);
        if ((framebufferInfo->flags & TX_DEPTH_TEST) && (framebufferInfo->flags & TX_DEPTH_BIT))
            framebufferInfo->framebuffers[framebufferInfo->currentFramebuffer][i].depth = framebufferInfo->depthClear;
    }
}

////////////////////////////////////////
TXpixel_t* txGetPixelFromCurrentFramebuffer(const TXframebufferInfo_t* framebufferInfo, int row, int col)
{
    int pos = row * framebufferInfo->width + col;
    if (pos < 0 || pos > framebufferInfo->width * framebufferInfo->height) {
        return NULL;
    }
    return &framebufferInfo->framebuffers[framebufferInfo->currentFramebuffer][pos];
}

////////////////////////////////////////
TXpixel_t* txGetPixelFromDisplayFramebuffer(const TXframebufferInfo_t* framebufferInfo, int row, int col)
{
    int pos = row * framebufferInfo->width + col;
    if (pos < 0 || pos > framebufferInfo->width * framebufferInfo->height) {
        return NULL;
    }
    return &framebufferInfo->framebuffers[!framebufferInfo->currentFramebuffer][pos];
}

////////////////////////////////////////
bool txSetPixelInCurrentFramebuffer(TXframebufferInfo_t* framebufferInfo, int row, int col, TXpixel_t* p)
{
    int pos = row * framebufferInfo->width + col;
    if (pos >= 0 && pos <= framebufferInfo->width * framebufferInfo->height) {
        txVec4Copy(framebufferInfo->framebuffers[framebufferInfo->currentFramebuffer][pos].color, p->color);
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////
bool txSetPixelInDisplayFramebuffer(TXframebufferInfo_t* framebufferInfo, int row, int col, TXpixel_t* p)
{
    int pos = row * framebufferInfo->width + col;
    if (pos >= 0 && pos <= framebufferInfo->width * framebufferInfo->height) {
        txVec4Copy(framebufferInfo->framebuffers[!framebufferInfo->currentFramebuffer][pos].color, p->color);
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////
void txDrawFramebuffer(TXappInfo_t* appInfo, TXframebufferInfo_t* framebufferInfo, int offsetX, int offsetY, int limitX, int limitY)
{
    for (int i = 0; i < framebufferInfo->height; ++i) {
        for (int j = 0; j < framebufferInfo->width; ++j) {
            TXpixel_t* currentPixel = txGetPixelFromCurrentFramebuffer(i, j);

            uint32_t u_r = (uint32_t)(currentPixel->color[0] * 255.0f);
            uint32_t u_g = (uint32_t)(currentPixel->color[1] * 255.0f);
            uint32_t u_b = (uint32_t)(currentPixel->color[2] * 255.0f);

            framebufferInfo->raw_framebuffer[i * framebufferInfo->width + j] = (((uint32_t)255 << 24) |
                                                                                          (u_b << 16) |
                                                                                          (u_g << 8)  |
                                                                                          (u_r << 0));
        }
    }
    ncblit_rgba(framebufferInfo->raw_framebuffer, framebufferInfo->width * (int)(sizeof(uint32_t)), &framebufferInfo->options);
    notcurses_render(appInfo->ctx);
}

////////////////////////////////////////
void txFreeFramebuffer(TXframebufferInfo_t* framebufferInfo)
{
    free(framebufferInfo->framebuffers[0]);
    free(framebufferInfo->framebuffers[1]);
    free(framebufferInfo->raw_framebuffer);
}
