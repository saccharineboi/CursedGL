// Copyright (C) 2023 saccharineboi

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
#define OUTPUT_FILE "hello_lines.txt"

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
/// example: hello_lines
////////////////////////////////////////
/// This example shows how to render lines
///
/// CONTROLS:
/// Q to quit
/// WASDZC and arrow keys to rotate
////////////////////////////////////////
int main(int argc, char** argv)
{
    txSetMessageCallback(messageCallback);
    if (!txInit(argc, argv))
        return ERR_INIT;

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txEnable(TX_DEPTH_TEST);

    txTranslate3f(0.0f, 0.0f, -1.0f);
    while (!processInput()) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        txViewportMax();
        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);


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

    txEnd();
    return SUCCESS;
}
