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
/// example: hello_lines
////////////////////////////////////////
/// This example shows how to render lines
///
/// CONTROLS:
/// Q to quit
/// WASDZC and arrow keys to rotate
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
    txEnable(TX_DEPTH_TEST);

    txPerspective(TX_PI / 2.0f,
                  txGetFramebufferAspectRatio(),
                  0.1f,
                  100.0f);

    txTranslate3f(0.0f, 0.0f, -1.0f);
    while (!processInput(input_win)) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        TXvec4 v0_begin = { -0.5f, -0.5f, 0.0f, 1.0f };
        TXvec4 v0_end   = {  0.5f, -0.5f, 0.0f, 1.0f };

        txColor3f(1.0f, 0.0f, 0.0f);
        txDrawLine(v0_begin, v0_end);

        TXvec4 v1_begin = { 0.5f, -0.5f, 0.0f, 1.0f };
        TXvec4 v1_end   = { 0.5f,  0.5f, 0.0f, 1.0f };

        txColor3f(0.0f, 1.0f, 0.0f);
        txDrawLine(v1_begin, v1_end);

        TXvec4 v2_begin = {  0.5f, 0.5f, 0.0f, 1.0f };
        TXvec4 v2_end   = { -0.5f, 0.5f, 0.0f, 1.0f };

        txColor3f(0.0f, 0.0f, 1.0f);
        txDrawLine(v2_begin, v2_end);

        TXvec4 v3_begin = { -0.5f,  0.5f, 0.0f, 1.0f };
        TXvec4 v3_end   = { -0.5f, -0.5f, 0.0f, 1.0f };

        txColor3f(1.0f, 1.0f, 0.0f);
        txDrawLine(v3_begin, v3_end);

        TXvec4 v4_begin = { -0.5f, -0.5f, 0.0f, 1.0f };
        TXvec4 v4_end   = {  0.5f,  0.5f, 0.0f, 1.0f };

        txColor3f(0.0f, 1.0f, 1.0f);
        txDrawLine(v4_begin, v4_end);

        TXvec4 v5_begin = { -0.5f,  0.5f, 0.0f, 1.0f };
        TXvec4 v5_end   = {  0.5f, -0.5f, 0.0f, 1.0f };

        txColor3f(1.0f, 0.0f, 1.0f);
        txDrawLine(v5_begin, v5_end);

        txSwapBuffers();
    }

    delwin(input_win);
    txEnd();
    return SUCCESS;
}
