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

#include "trex.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

////////////////////////////////////////
#define SUCCESS  0
#define ERR_INIT 1
#define ERR_EXIT 2
#define ERR_LOAD 3
#define ROTATION_SPEED 0.05f

////////////////////////////////////////
/// Always use a different ncurses window
/// for input. This is because ncurses
/// implicitly calls refresh() after every
/// getch() invocation, causing a performance
/// hit if getch is called on the window
/// TRex is rendering into
////////////////////////////////////////
static WINDOW* createInputWindow()
{
    WINDOW* input_win = newwin(0, 0, 0, 0);
    raw();
    noecho();
    nodelay(input_win, true);
    keypad(input_win, true);
    return input_win;
}

////////////////////////////////////////
/// Basic input processing without
/// finite-state automaton. This works
/// because TRex is itself a finite-state
/// automaton, much like OpenGL
////////////////////////////////////////
static bool processInput(WINDOW* win)
{
    int k = wgetch(win);
    switch (k) {
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
        case KEY_LEFT:
            txMatrixMode(TX_LIGHT);
            txRotate4f(-ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case KEY_RIGHT:
            txMatrixMode(TX_LIGHT);
            txRotate4f(ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case KEY_UP:
            txMatrixMode(TX_LIGHT);
            txRotate4f(-ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case KEY_DOWN:
            txMatrixMode(TX_LIGHT);
            txRotate4f(ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
    }
    return false;
}

////////////////////////////////////////
/// example: teapot
////////////////////////////////////////
/// This example shows how to render
/// a 3D object with dynamic lighting.
///
/// CONTROLS:
/// Q to quit
/// WASDZC to rotate the object
/// Arrow keys to rotate the light
////////////////////////////////////////
int main(void)
{
    if (!txInit(TX_BLOCK_MODE)) {
        fprintf(stderr, "ERROR: couldn't initialize TRex\n");
        return ERR_INIT;
    }

    WINDOW* input_win = createInputWindow();

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txViewportMax();
    txEnable(TX_DEPTH_TEST | TX_CULL_FACE);

    txPerspective(TX_PI / 2.0f,
                  txGetFramebufferAspectRatio(),
                  0.1f,
                  100.0f);

    TXobjModel_t teapot;
    if (!txLoadObjModel("../obj/utah-teapot.obj", &teapot)) {
        txEnd();
        fprintf(stderr, "ERROR: couldn't load the obj file");
        return ERR_LOAD;
    }

    txMaterial3f(TX_MATERIAL_AMBIENT, 1.0f, 1.0f, 1.0f);
    txMaterial3f(TX_MATERIAL_DIFFUSE, 1.0f, 1.0f, 1.0f);
    txMaterial3f(TX_MATERIAL_SPECULAR, 1.0f, 1.0f, 1.0f);
    txMaterial1f(TX_MATERIAL_SHININESS, 32.0f);

    txLight3f(0, TX_LIGHT_DIRECTIONAL, TX_LIGHT_DIRECTION, 0.0f, 0.0f, -1.0f);
    txLight3f(0, TX_LIGHT_DIRECTIONAL, TX_LIGHT_AMBIENT, 0.2f, 0.2f, 0.2f);
    txLight3f(0, TX_LIGHT_DIRECTIONAL, TX_LIGHT_DIFFUSE, 0.0f, 1.0f, 1.0f);
    txLight3f(0, TX_LIGHT_DIRECTIONAL, TX_LIGHT_SPECULAR, 1.0f, 1.0f, 1.0f);
    txLight1f(0, TX_LIGHT_DIRECTIONAL, TX_LIGHT_INTENSITY, 1.0f);

    txShadeModel(TX_SMOOTH);

    txTranslate3f(0.0f, 0.0f, -2.0f);
    while (!processInput(input_win)) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);
        txRenderObjModel(&teapot);
        txSwapBuffers();
    }

    delwin(input_win);
    txFreeObjModel(&teapot);
    txEnd();
    return SUCCESS;
}
