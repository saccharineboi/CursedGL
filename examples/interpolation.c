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
        case KEY_LEFT:
            txRotate4f(ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'd':
        case KEY_RIGHT:
            txRotate4f(-ROTATION_SPEED, 0.0f, 1.0f, 0.0f);
            break;
        case 'w':
        case KEY_UP:
            txRotate4f(-ROTATION_SPEED, 1.0f, 0.0f, 0.0f);
            break;
        case 's':
        case KEY_DOWN:
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
/// This example shows how TRex interpolates
/// vertex attributes on a single triangle
///
/// CONTROLS:
/// Q to quit
/// WASDZC and arrow keys to rotate
////////////////////////////////////////
int main(void)
{
    if (!(txInit() && txSetRenderWindow(stdscr, TX_BLOCK_MODE))) {
        fprintf(stderr, "ERROR: couldn't initialize TRex\n");
        return ERR_INIT;
    }
    WINDOW* input_win = createInputWindow();

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txViewportMax();

    txPerspective(TX_PI / 2.0f,
                  txGetFramebufferAspectRatio(),
                  0.1f,
                  100.0f);

    txTranslate3f(0.0f, 0.0f, -2.0f);
    while (!processInput(input_win)) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        TXvec4 pos0[] = { { -1.0f, -1.0f, 0.0f, 1.0f},
                          {  1.0f,  0.0f, 0.0f, 1.0f } };
        TXvec4 pos1[] = { {  1.0f, -1.0f, 0.0f, 1.0f},
                          {  0.0f,  1.0f, 0.0f, 1.0f } };
        TXvec4 pos2[] = { {  0.0f,  1.0f, 0.0f, 1.0f},
                          {  0.0f,  0.0f, 1.0f, 1.0f } };

        txDrawTriangle(pos0, pos1, pos2, TX_POSITION_COLOR);

        txSwapBuffers();
    }

    delwin(input_win);
    txEnd();
    return SUCCESS;
}
