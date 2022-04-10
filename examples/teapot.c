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
#define OUTPUT_FILE "teapot.txt"

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
    FILE* outputFile = fopen(OUTPUT_FILE, "a");
    if (outputFile) {
        switch (type) {
            case TX_INFO:
                fprintf(outputFile, "[INFO] %s\n", message);
                break;
            case TX_WARNING:
                fprintf(outputFile, "[WARNING] %s\n", message);
                break;
            case TX_ERROR:
                fprintf(outputFile, "[ERROR] %s\n", message);
                break;
        }
        fclose(outputFile);
    }
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
int main()
{
    txSetMessageCallback(messageCallback);
    if (!txInit())
        return ERR_INIT;

    txClearColor3f(0.0f, 0.0f, 0.0f);
    txEnable(TX_DEPTH_TEST | TX_CULL_FACE);

    TXobjModel_t teapot;
    if (!txLoadObjModel("../obj/utah-teapot.obj", &teapot))
        return ERR_LOAD;

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
    while (!processInput()) {
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        txViewportMax();
        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);

        txRenderObjModel(&teapot);
        txSwapBuffers();
    }

    txFreeObjModel(&teapot);
    txEnd();
    return SUCCESS;
}
