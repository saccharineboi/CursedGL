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
/// Exit the game loop if the user
/// presses q
////////////////////////////////////////
static bool processInput()
{
    struct ncinput input;
    notcurses_get_nblock(txGetContext(), &input);

    if (input.id == 'q')
        return true;
    return false;
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
int main()
{
    if (!txInit()) {
        fprintf(stderr, "ERROR: couldn't initialize CursedGL\n");
        return ERR_INIT;
    }
    srandom((unsigned)time(NULL));

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txViewportMax();

    txOrtho(1.0f, 1.0f, -1.0f, 1.0f);

    // make CursedGL run as fast as it can
    txSetWaitMilliseconds(0);

    int numInsidePoints = 0;
    int numOutsidePoints = 0;
    while (!processInput()) {

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

    txEnd();

    int numTotalPoints = numInsidePoints + numOutsidePoints;
    printf("Total # of points: %d\n", numTotalPoints);
    printf("π is estimated to be %f\n", ((float)numInsidePoints / (float)numTotalPoints) * 4.0f);
    return SUCCESS;
}
