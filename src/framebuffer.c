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
static int depthFunc = TX_LESS;

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
            txOutputError("CursedGL: depthFunc is invalid");
            return false;
    }
}

////////////////////////////////////////
void txDepthFunc(int func)
{
    depthFunc = func;
}

////////////////////////////////////////
int txGetDepthFunc()
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
float txGetFramebufferAspectRatio()
{
    return (float)framebufferWidth / (float)framebufferHeight;
}

////////////////////////////////////////
void txViewportMax()
{
    int maxWidth, maxHeight;
    txGetFramebufferMaxDims(&maxWidth, &maxHeight);
    txViewport(maxWidth, maxHeight);
}

////////////////////////////////////////
void txViewport(int width, int height)
{
    if (framebufferWidth   != width   ||
        framebufferHeight  != height) {

        newFramebufferWidth     = width;
        newFramebufferHeight    = height;

        viewportResized         = true;
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
        txOutputError("CursedGL: (row, col) is invalid");
        return NULL;
    }
    return &framebuffers[type][pos];
}

////////////////////////////////////////
void txSetPixelInFramebuffer(int row, int col, TXpixel_t* p, enum TXframebufferType type)
{
    int pos = row * framebufferWidth + col;
    if (pos >= 0 && pos <= framebufferWidth * framebufferHeight)
        txVec4Copy(framebuffers[type][pos].color, p->color);
}

////////////////////////////////////////
/// This function is running continuously
/// on a different thread, and it will
/// continue to run until the swap thread
/// (aka the main thread) tells it to stop.
///
/// This is where the contents of the framebuffer
/// is given to ncurses to be rendered to the
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

        struct ncvisual_options options = {
            .n          = renderPlane,
            .scaling    = NCSCALE_NONE,
            .leny       = (unsigned)framebufferHeight,
            .lenx       = (unsigned)framebufferWidth,
            .blitter    = NCBLIT_1x1
        };

        currentlyRendering = true;
        int cnt = 0;
        for (int i = 0; i < framebufferHeight; ++i) {
            for (int j = 0; j < framebufferWidth; ++j) {
                TXpixel_t* currentPixel = txGetPixelFromFrontFramebuffer(i, j);
                uint32_t u_r = (uint32_t)(currentPixel->color[0] * 255.0f);
                uint32_t u_g = (uint32_t)(currentPixel->color[1] * 255.0f);
                uint32_t u_b = (uint32_t)(currentPixel->color[2] * 255.0f);

                raw_framebuffer[cnt++] = (((uint32_t)255 << 24) |
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
    if (pthread_join(renderThread, NULL))
        return false;

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
            txOutputError("CursedGL: couldn't allocate enough memory for front framebuffer");
        }

        framebuffers[BACK_BUFFER] = (TXpixel_t*)malloc(txGetFramebufferSize());
        if (!framebuffers[BACK_BUFFER]) {
            txOutputError("CursedGL: couldn't allocate enough memory for back framebuffer");
        }

        raw_framebuffer = (uint32_t*)malloc(txGetRawFramebufferSize());
        if (!raw_framebuffer) {
            txOutputError("CursedGL: couldn't allocate enough memory for raw framebuffer");
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
        txOutputError("CursedGL: couldn't create renderthread");
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
