// Copyright (C) 2022 saccharineboi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "framebuffer.h"
#include "pixel.h"
#include "colors.h"

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>

////////////////////////////////////////
static TXpixel_t* framebuffers[2];

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
static int framebufferOffsetX;

////////////////////////////////////////
static int framebufferOffsetY;

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
static WINDOW* renderWindow;

////////////////////////////////////////
static bool viewportResized;

////////////////////////////////////////
WINDOW* txGetRenderWindow()
{
    return renderWindow;
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
    return (size_t)((framebufferWidth  - framebufferOffsetX) *
                    (framebufferHeight - framebufferOffsetY) *
                    (int)sizeof(TXpixel_t));
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
int txGetFramebufferMaxWidth()
{
    return getmaxx(renderWindow);
}

////////////////////////////////////////
int txGetFramebufferMaxHeight()
{
    return getmaxy(renderWindow);
}

////////////////////////////////////////
int txGetFramebufferOffsetX()
{
    return framebufferOffsetX;
}

////////////////////////////////////////
int txGetFramebufferOffsetY()
{
    return framebufferOffsetY;
}

////////////////////////////////////////
float txGetFramebufferAspectRatio()
{
    return (float)framebufferWidth / (float)framebufferHeight;
}

////////////////////////////////////////
bool txViewportMax()
{
    int maxWidth, maxHeight;
    getmaxyx(renderWindow, maxHeight, maxWidth);
    return txViewport(0, 0, maxWidth, maxHeight);
}

////////////////////////////////////////
bool txViewport(int offsetX, int offsetY, int width, int height)
{
    if (framebufferOffsetX != offsetX ||
        framebufferOffsetY != offsetY ||
        framebufferWidth   != width   ||
        framebufferHeight  != height) {

        viewportResized = true;

        framebufferOffsetX  = offsetX;
        framebufferOffsetY  = offsetY;
        framebufferWidth    = width;
        framebufferHeight   = height;

        free(framebuffers[BACK_BUFFER]);
        framebuffers[BACK_BUFFER] = (TXpixel_t*)malloc(txGetFramebufferSize());
        if (!framebuffers[BACK_BUFFER]) {
            endwin();
            fprintf(stderr, "ERROR: not enough memory for back buffer\n");
            return false;
        }
    }
    return true;
}

////////////////////////////////////////
void txGetFramebufferDims(int* width, int* height)
{
    *width  = framebufferWidth;
    *height = framebufferHeight;
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
    int pos = row * framebufferHeight + col;
    if (pos < 0 || pos > framebufferWidth * framebufferHeight)
        return NULL;
    return &framebuffers[type][pos];
}

////////////////////////////////////////
void txSetPixelInFramebuffer(int row, int col, TXpixel_t* p, enum TXframebufferType type)
{
    int pos = row * framebufferHeight + col;
    if (pos >= 0 && pos <= framebufferWidth * framebufferHeight)
        txVec4Copy(framebuffers[type][pos].color, p->color);
}

////////////////////////////////////////
/// The effect of this function is visible
/// only when TRex is running in ASCII mode
////////////////////////////////////////
static unsigned selectLuminanceChar(TXvec3 color)
{
    static unsigned luminanceValues[] = { '.', ',', '-', '~', ':', ';', '=', '!', '*', '#', '$', '#' };
    float sum = color[0] + color[1] + color[2];
    return luminanceValues[(int)(sum * 4.0f - 1.0f)];
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

        currentlyRendering = true;
        for (int i = framebufferOffsetX; i < framebufferWidth; ++i) {
            for (int j = framebufferOffsetY; j < framebufferHeight; ++j) {
                TXpixel_t* currentPixel = txGetPixelFromFrontFramebuffer(i, j);
                txSetColor(currentPixel->color[0], currentPixel->color[1], currentPixel->color[2]);
                mvwaddch(renderWindow, j, i, selectLuminanceChar(currentPixel->color));
            }
        }
        if (!viewportResized)
            wrefresh(renderWindow);
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
        framebuffers[FRONT_BUFFER] = (TXpixel_t*)malloc(txGetFramebufferSize());
        if (!framebuffers[FRONT_BUFFER]) {
            endwin();
            fprintf(stderr, "not enough memory for front buffer\n");
            return;
        }
        viewportResized = false;
        return;
    }

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

////////////////////////////////////////
static void setSwapRenderThreadWaits()
{
    swapThreadWait   = (unsigned)(waitMilliseconds * 1000);
    renderThreadWait = swapThreadWait / swapToRenderRatio;
}

////////////////////////////////////////
bool txInitFramebuffer(WINDOW* window)
{
    renderWindow = window;
    if (pthread_create(&renderThread, NULL, drawFramebuffer, NULL))
        return false;
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
