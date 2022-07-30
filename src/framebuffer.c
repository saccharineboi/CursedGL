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
static TXpixel_t* framebuffers[2];

////////////////////////////////////////
static uint32_t* raw_framebuffer = NULL;

////////////////////////////////////////
int currentFramebuffer = TX_FRAMEBUFFER_FRONT;

////////////////////////////////////////
#define FRONT_BUFFER currentFramebuffer

////////////////////////////////////////
#define BACK_BUFFER !currentFramebuffer

////////////////////////////////////////
static int framebufferWidth;

////////////////////////////////////////
static int framebufferHeight;

////////////////////////////////////////
static TXvec4 framebufferClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

////////////////////////////////////////
/// This roughly corresponds to 58.8 FPS,
/// you may tweak this for monitors with
/// lower or higher refresh rates
////////////////////////////////////////
static unsigned waitMilliseconds = 17;

////////////////////////////////////////
static unsigned swapThreadWait;

////////////////////////////////////////
static unsigned renderThreadWait;

////////////////////////////////////////
static pthread_t renderThread;

////////////////////////////////////////
static bool startRendering = false;

////////////////////////////////////////
static bool stopRendering = false;

////////////////////////////////////////
static bool currentlyRendering = false;

////////////////////////////////////////
static unsigned swapToRenderRatio = 10;

////////////////////////////////////////
static unsigned stateFlags;

////////////////////////////////////////
static enum TXdepthFunc depthFunc = TX_LESS;

////////////////////////////////////////
static bool depthMask = true;

////////////////////////////////////////
static float depthClear = 1.0f;

////////////////////////////////////////
static struct ncplane* renderPlane;

////////////////////////////////////////
static bool viewportResized;

////////////////////////////////////////
static int newFramebufferWidth;

////////////////////////////////////////
static int newFramebufferHeight;

////////////////////////////////////////
struct ncplane* txGetRenderPlane()
{
    return renderPlane;
}

////////////////////////////////////////
void txClearDepth(float value)
{
    depthClear = value;
}

////////////////////////////////////////
float txGetClearDepth()
{
    return depthClear;
}

////////////////////////////////////////
bool txCompareDepth(float interpolatedDepth, float pixelDepth)
{
    switch (depthFunc) {
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
            txOutputMessage(TX_WARNING, "[CursedGL] txCompareDepth: %d is not a valid depthFunc", depthFunc);
            return false;
    }
}

////////////////////////////////////////
void txDepthFunc(enum TXdepthFunc func)
{
    depthFunc = func;
}

////////////////////////////////////////
enum TXdepthFunc txGetDepthFunc()
{
    return depthFunc;
}

////////////////////////////////////////
unsigned txGetSwapThreadWait()
{
    return swapThreadWait;
}

////////////////////////////////////////
unsigned txGetRenderThreadWait()
{
    return renderThreadWait;
}

////////////////////////////////////////
bool txIsCurrentlyRendering()
{
    return currentlyRendering;
}

////////////////////////////////////////
void txSetSwapToRenderRatio(unsigned ratio)
{
    swapToRenderRatio = ratio;
}

////////////////////////////////////////
size_t txGetFramebufferSize()
{
    return (size_t)(framebufferWidth * framebufferHeight * (int)sizeof(TXpixel_t));
}

////////////////////////////////////////
size_t txGetRawFramebufferSize()
{
    return (size_t)(framebufferWidth * framebufferHeight * (int)sizeof(uint32_t));
}

////////////////////////////////////////
int txGetFramebufferWidth()
{
    return framebufferWidth;
}

////////////////////////////////////////
int txGetFramebufferHeight()
{
    return framebufferHeight;
}

////////////////////////////////////////
static void getActualDims(ncblitter_e blitter, int* actualWidth,
                                               int* actualHeight,
                                               int effectiveWidth,
                                               int effectiveHeight)
{
    switch (blitter) {
        case NCBLIT_1x1:
            *actualWidth = effectiveWidth;
            *actualHeight = effectiveHeight;
            break;
        case NCBLIT_2x1:
            *actualWidth = effectiveWidth;
            *actualHeight = effectiveHeight / 2;
            break;
        case NCBLIT_2x2:
            *actualWidth = effectiveWidth / 2;
            *actualHeight = effectiveHeight / 2;
            break;
        case NCBLIT_3x2:
            *actualWidth = effectiveWidth / 2;
            *actualHeight = effectiveHeight / 3;
            break;
        case NCBLIT_BRAILLE:
            *actualWidth = effectiveWidth / 2;
            *actualHeight = effectiveHeight / 4;
            break;
        case NCBLIT_PIXEL:
        case NCBLIT_DEFAULT:
        case NCBLIT_4x1:
        case NCBLIT_8x1:
            break;
    }
}

////////////////////////////////////////
float txGetFramebufferAspectRatio()
{
    ncblitter_e currentBlitter = txGetCurrentBlitter();
    int actualWidth = 0, actualHeight = 0;
    getActualDims(currentBlitter, &actualWidth,
                                  &actualHeight,
                                  framebufferWidth,
                                  framebufferHeight);
    return (float)actualWidth / (float)actualHeight;
}

////////////////////////////////////////
void txViewportMax()
{
    int maxWidth, maxHeight;
    txGetFramebufferMaxDims(&maxWidth, &maxHeight);
    txViewport(maxWidth, maxHeight);
}

////////////////////////////////////////
static void getEffectiveDims(ncblitter_e blitter, int* effectiveWidth,
                                                  int* effectiveHeight,
                                                  int actualWidth,
                                                  int actualHeight)
{
    switch (blitter) {
        case NCBLIT_1x1:
            *effectiveWidth = actualWidth;
            *effectiveHeight = actualHeight;
            break;
        case NCBLIT_2x1:
            *effectiveWidth = actualWidth;
            *effectiveHeight = actualHeight * 2;
            break;
        case NCBLIT_2x2:
            *effectiveWidth = actualWidth * 2;
            *effectiveHeight = actualHeight * 2;
            break;
        case NCBLIT_3x2:
            *effectiveWidth = actualWidth * 2;
            *effectiveHeight = actualHeight * 3;
            break;
        case NCBLIT_BRAILLE:
            *effectiveWidth = actualWidth * 2;
            *effectiveHeight = actualHeight * 4;
            break;
        case NCBLIT_PIXEL:
        case NCBLIT_DEFAULT:
        case NCBLIT_4x1:
        case NCBLIT_8x1:
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
void txGetFramebufferDims(int* width, int* height)
{
    *width  = framebufferWidth;
    *height = framebufferHeight;
}

////////////////////////////////////////
void txGetFramebufferMaxDims(int* width, int* height)
{
    ncplane_dim_yx(renderPlane, (unsigned*)height, (unsigned*)width);
}

////////////////////////////////////////
void txClearColor4f(float r, float g, float b, float a)
{
    framebufferClearColor[0] = r;
    framebufferClearColor[1] = g;
    framebufferClearColor[2] = b;
    framebufferClearColor[3] = a;
}

////////////////////////////////////////
void txClearColor4fv(TXvec4 color)
{
    txVec4Copy(framebufferClearColor, color);
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

////////////////////////////////////////
bool txFreeFramebuffer()
{
    while (currentlyRendering)
        usleep(renderThreadWait);

    stopRendering = true;
    if (pthread_join(renderThread, NULL)) {
        txOutputMessage(TX_ERROR, "[CursedGL] txFreeFramebuffer: pthread_join failed for renderThread");
        return false;
    }

    free(framebuffers[FRONT_BUFFER]);
    free(framebuffers[BACK_BUFFER]);

    free(raw_framebuffer);

    framebufferWidth = framebufferHeight = 0;
    return true;
}

////////////////////////////////////////
void txSwapBuffers()
{
    static unsigned timeWaited;
    while (currentlyRendering) {
        usleep(renderThreadWait);
        timeWaited += renderThreadWait;
    }

    if (viewportResized) {
        free(framebuffers[FRONT_BUFFER]);
        free(framebuffers[BACK_BUFFER]);

        free(raw_framebuffer);

        framebufferWidth = newFramebufferWidth;
        framebufferHeight = newFramebufferHeight;

        framebuffers[FRONT_BUFFER] = (TXpixel_t*)malloc(txGetFramebufferSize());
        if (!framebuffers[FRONT_BUFFER]) {
            txOutputMessage(TX_ERROR, "[CursedGL] txSwapBuffers: couldn't allocate enough memory for front framebuffer");
        }

        framebuffers[BACK_BUFFER] = (TXpixel_t*)malloc(txGetFramebufferSize());
        if (!framebuffers[BACK_BUFFER]) {
            txOutputMessage(TX_ERROR, "[CursedGL] txSwapBuffers: couldn't allocate enough memory for back framebuffer");
        }

        raw_framebuffer = (uint32_t*)malloc(txGetRawFramebufferSize());
        if (!raw_framebuffer) {
            txOutputMessage(TX_ERROR, "[CursedGL] txSwapBuffers: couldn't allocate enough memory for raw framebuffer");
        }

        notcurses_refresh(txGetContext(), NULL, NULL);

        viewportResized = false;
        startRendering = true;

        return;
    }
    else {
        memcpy(framebuffers[FRONT_BUFFER],
               framebuffers[BACK_BUFFER],
               txGetFramebufferSize());

        currentFramebuffer ^= 1;
        startRendering = true;

        unsigned timeToWait = swapThreadWait - timeWaited;
        if (timeToWait > swapThreadWait)
            timeToWait = 0;
        timeWaited = 0;
        usleep(timeToWait);
    }
}

////////////////////////////////////////
static void setSwapRenderThreadWaits()
{
    swapThreadWait   = (unsigned)(waitMilliseconds * 1000);
    renderThreadWait = swapThreadWait / swapToRenderRatio;
}

////////////////////////////////////////
bool txInitFramebuffer(struct ncplane* plane)
{
    renderPlane = plane;
    if (pthread_create(&renderThread, NULL, drawFramebuffer, NULL)) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitFramebuffer: couldn't create renderThread");
        return false;
    }
    setSwapRenderThreadWaits();
    return true;
}

////////////////////////////////////////
enum TXframebufferType txGetFrontBuffer()
{
    return currentFramebuffer;
}

////////////////////////////////////////
enum TXframebufferType txGetBackBuffer()
{
    return !currentFramebuffer;
}

////////////////////////////////////////
void txSetWaitMilliseconds(unsigned milliseconds)
{
    waitMilliseconds = milliseconds;
    setSwapRenderThreadWaits();
}

////////////////////////////////////////
unsigned txGetWaitMilliseconds()
{
    return waitMilliseconds;
}

////////////////////////////////////////
void txEnable(unsigned flags)
{
    stateFlags |= flags;
}

////////////////////////////////////////
void txDisable(unsigned flags)
{
    stateFlags &= ~flags;
}

////////////////////////////////////////
bool txIsDepthTestEnabled()
{
    return stateFlags & TX_DEPTH_TEST;
}

////////////////////////////////////////
bool txIsCullingEnabled()
{
    return stateFlags & TX_CULL_FACE;
}

////////////////////////////////////////
void txDepthMask(bool flag)
{
    depthMask = flag;
}

////////////////////////////////////////
bool txGetDepthMask()
{
    return depthMask;
}
