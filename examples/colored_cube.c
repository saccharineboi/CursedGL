// Copyright (C) 2022 saccharineboi

#include <cursedgl.h>

#include <notcurses/notcurses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

////////////////////////////////////////
#define ERR_INIT 1
#define ERR_EXIT 2
#define ERR_LOAD 3
#define ERR_MODE 4
#define ROTATION_SPEED 0.05f
#define OUTPUT_FILE "colored_cube.txt"

////////////////////////////////////////
/// Basic input processing without
/// finite-state automaton. This works
/// because CursedGL is itself a finite-state
/// automaton, much like OpenGL
////////////////////////////////////////
static bool processInput()
{
    struct ncinput input;
    notcurses_get_nblock(txGetContext(), &input);

    switch (input.id) {
        case 'q':
            return true;
        case 'a':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'd':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(-ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'w':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(-ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case 's':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case 'z':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(ROTATION_SPEED, 0.0f, 0.0f, 1.0f);
            break;
        case 'c':
            txMatrixMode(TX_MODELVIEW);
            txRotate4f(-ROTATION_SPEED, 0.0f, 0.0f, 1.0f);
            break;
        case NCKEY_LEFT:
            txMatrixMode(TX_LIGHT);
            txRotate4f(-ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case NCKEY_RIGHT:
            txMatrixMode(TX_LIGHT);
            txRotate4f(ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case NCKEY_UP:
            txMatrixMode(TX_LIGHT);
            txRotate4f(-ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case NCKEY_DOWN:
            txMatrixMode(TX_LIGHT);
            txRotate4f(ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
    }
    return false;
}

////////////////////////////////////////
/// Callback used for getting information
/// from CursedGL
////////////////////////////////////////
static void messageCallback(enum TXmessageType type, const char* message)
{
    switch (type) {
        case TX_INFO:
            fprintf(stderr, "[INFO] %s\n", message);
            break;
        case TX_WARNING:
            fprintf(stderr, "[WARNING] %s\n", message);
            break;
        case TX_ERROR:
            fprintf(stderr, "[ERROR] %s\n", message);
            break;
    }
}

////////////////////////////////////////
/// This example shows how to render
/// a 3D object
///
/// CONTROLS:
/// Q to quit
/// WASDZC to rotate the object
/// Arrow keys to rotate the light
////////////////////////////////////////
int main(int argc, char** argv)
{
    if (!setlocale(LC_ALL, "")) {
        fprintf(stderr, "ERROR: couldn't set locale\n");
        exit(EXIT_FAILURE);
    }

    TXappInfo_t appInfo = {
        .options = {
            .termType = NULL,
            .loglevel = NCLOGLEVEL_WARNING,

            .margin_t = 0,
            .margin_r = 0,
            .margin_b = 0,
            .margin_l = 0,

            .flags = NCOPTION_SUPPRESS_BANNERS
        },
        .blitter = NCBLIT_1x1,
        .messageCallback = messageCallback
    };

    appInfo.ctx = notcurses_core_init(&appInfo.options, NULL);
    if (!appInfo.ctx) {
        fprintf(stderr, "ERROR: failed to initialize notcurses\n");
        exit(EXIT_FAILURE);
    }

    TXframebufferInfo_t framebufferInfo = {
        .framebuffers = { NULL, NULL },
        .clearColor = { 0.2f, 0.3f, 0.3f, 1.0f },

        .flags = TX_DEPTH_TEST | TX_CULL_FACE,

        .depthFunc = TX_LESS,
        .depthMask = true,
        .depthClear = 1.0f,

        .renderPlane = notcurses_stdplane(appInfo.ctx)
    };

    framebufferInfo.options = {
        .n = framebufferInfo.renderPlane,
        .scaling = NCSCALE_NONE,
        .leny = (unsigned)framebufferInfo.height,
        .lenx = (unsigned)framebufferInfo.width,
        .blitter = appInfo.blitter
    };

    while (!processInput()) {

        int plane_dim_x, plane_dim_y;
        ncplane_dim_yx(framebufferInfo.renderPlane, (unsigned*)&plane_dim_y, (unsigned*)&plane_dim_x);

        txViewport(&appInfo, &framebufferInfo, plane_dim_x, plane_dim_y);

        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);

        txDrawColoredCube();
    }

    txFreeFramebuffer(&framebufferInfo);
    return notcurses_stop(appInfo.ctx);
}
