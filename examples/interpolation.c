// Copyright (C) 2022 saccharineboi

#include <cursedgl.h>

#include <notcurses/notcurses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

////////////////////////////////////////
#define SUCCESS  0
#define ERR_INIT 1
#define ERR_EXIT 2
#define ERR_LOAD 3
#define ERR_MODE 4
#define ROTATION_SPEED 0.05f

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
        case NCKEY_LEFT:
            txRotate4f(ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'd':
        case NCKEY_RIGHT:
            txRotate4f(-ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'w':
        case NCKEY_UP:
            txRotate4f(-ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case 's':
        case NCKEY_DOWN:
            txRotate4f(ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case 'z':
            txRotate4f(ROTATION_SPEED, 0.0f, 0.0f, 1.0f);
            break;
        case 'c':
            txRotate4f(-ROTATION_SPEED, 0.0f, 0.0f, 1.0f);
            break;
    }
    return false;
}

////////////////////////////////////////
/// example: interpolation
////////////////////////////////////////
/// This example shows how CursedGL interpolates
/// vertex attributes on a single triangle
///
/// CONTROLS:
/// Q to quit
/// WASDZC and arrow keys to rotate
////////////////////////////////////////
int main()
{
    if (!txInit()) {
        fprintf(stderr, "ERROR: couldn't initialize CursedGL\n");
        return ERR_INIT;
    }

    txClearColor3f(0.0f, 0.0f, 0.0f);

    txTranslate3f(0.0f, 0.0f, -2.0f);
    while (!processInput()) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        txViewportMax();
        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);

        TXvec4 pos0[] = { { -1.0f, -1.0f, 0.0f, 1.0f},
                          {  1.0f,  0.0f, 0.0f, 1.0f } };
        TXvec4 pos1[] = { {  1.0f, -1.0f, 0.0f, 1.0f},
                          {  0.0f,  1.0f, 0.0f, 1.0f } };
        TXvec4 pos2[] = { {  0.0f,  1.0f, 0.0f, 1.0f},
                          {  0.0f,  0.0f, 1.0f, 1.0f } };

        txDrawTriangle(pos0, pos1, pos2, TX_POSITION_COLOR);

        txSwapBuffers();
    }

    txEnd();
    return SUCCESS;
}
