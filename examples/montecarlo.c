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
#define ERR_MODE 4
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
/// Exit the game loop if the user
/// presses q
////////////////////////////////////////
static bool processInput(WINDOW* win)
{
    if (wgetch(win) == 'q')
        return true;
    return false;
}

////////////////////////////////////////
static int getModeFromUser(int argc, char** argv)
{
    if (argc < 2)
        return TX_BLOCK_MODE;
    if (!strcmp(argv[1], "ascii"))
        return TX_ASCII_MODE;
    else if (!strcmp(argv[1], "block"))
        return TX_BLOCK_MODE;
    else
        return -1;
}

////////////////////////////////////////
/// example: montecarlo
////////////////////////////////////////
/// This 2D example shows how to use points
///
/// Inspired by Joma's favorite interview
/// question: https://youtu.be/pvimAM_SLic
///
/// CONTROLS:
/// Q to quit
////////////////////////////////////////
int main(int argc, char** argv)
{
    int mode = getModeFromUser(argc, argv);
    if (mode == -1) {
        fprintf(stderr, "ERROR: invalid mode\n");
        return ERR_MODE;
    }

    if (!(txInit() && txSetRenderWindow(stdscr, mode))) {
        fprintf(stderr, "ERROR: couldn't initialize TRex\n");
        return ERR_INIT;
    }
    srandom((unsigned)time(NULL));
    WINDOW* input_win = createInputWindow();

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txViewportMax();

    txOrtho(1.0f, 1.0f, -1.0f, 1.0f);

    // make TRex run as fast as it can
    txSetWaitMilliseconds(0);

    int numInsidePoints = 0;
    int numOutsidePoints = 0;
    while (!processInput(input_win)) {

        // note the absence of txClear

        long rand_x = random() % 1000;
        long rand_y = random() % 1000;

        float pos_x = (float)rand_x / 1000.0f;
        float pos_y = (float)rand_y / 1000.0f;

        pos_x = 1.0f - pos_x * 2.0f;
        pos_y = 1.0f - pos_y * 2.0f;

        TXvec4 pos = { pos_x, pos_y, 0.0f, 1.0f };

        long rand_r = random() % 10;
        long rand_g = random() % 10;
        long rand_b = random() % 10;

        float col_r = (float)rand_r / 10.0f;
        float col_g = (float)rand_g / 10.0f;
        float col_b = (float)rand_b / 10.0f;

        if (txVec3Len(pos) <= 1.0f) {
            txColor3f(0.0f, col_g, col_b);
            ++numInsidePoints;
        }
        else {
            txColor3f(col_r, col_g, 0.0f);
            ++numOutsidePoints;
        }

        txDrawPoint(pos);
        txSwapBuffers();
    }

    delwin(input_win);
    txEnd();

    int numTotalPoints = numInsidePoints + numOutsidePoints;
    printf("Total # of points: %d\n", numTotalPoints);
    printf("π is estimated to be %f\n", ((float)numInsidePoints / (float)numTotalPoints) * 4.0f);
    return SUCCESS;
}
