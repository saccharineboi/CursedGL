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
#define SUCCESS                 0
#define ERR_INIT                1
#define ERR_EXIT                2
#define ERR_LOAD                3
#define CAMERA_SPEED            0.25f
#define ROTATION_SPEED          0.05f
#define DIR_LIGHT_COLOR_R       1.0f
#define DIR_LIGHT_COLOR_G       1.0f
#define DIR_LIGHT_COLOR_B       1.0f
#define POINT_LIGHT_COLOR_R     0.0f
#define POINT_LIGHT_COLOR_G     1.0f
#define POINT_LIGHT_COLOR_B     1.0f
#define POINT_LIGHT_RADIUS      10.0f
#define POINT_LIGHT_HEIGHT      5.0f
#define SPOT_LIGHT_COLOR_R      1.0f
#define SPOT_LIGHT_COLOR_G      1.0f
#define SPOT_LIGHT_COLOR_B      1.0f
#define CUBE_SCALE              3.0f
#define CUBE_POS                10.0f

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
/// Basic implementation of a first-person
/// camera. Use W and S to move forward and
/// backward, A and D to look left and right,
/// H and L to strafe left and right,
/// Z and C to move up and down
////////////////////////////////////////
static bool processInput(WINDOW* win)
{
    static TXvec3 eye       = { 0.0f, 0.0f, 10.0f };
    static TXvec3 up        = { 0.0f, CAMERA_SPEED, 0.0f };
    static TXvec3 direction = { 0.0f, 0.0f, -CAMERA_SPEED };
    static bool flashLightEnabled;
    TXvec3 strafe;

    int k = wgetch(win);
    switch (k) {
        case 'q':
            return true;
        case 'w':
        case 'k':
            txVec3Add(eye, eye, direction);
            break;
        case 's':
        case 'j':
            txVec3Sub(eye, eye, direction);
            break;
        case 'a':
            txVec3RotateY(direction, direction, ROTATION_SPEED);
            break;
        case 'd':
            txVec3RotateY(direction, direction, -ROTATION_SPEED);
            break;
        case 'z':
            txVec3Add(eye, eye, up);
            break;
        case 'c':
            txVec3Sub(eye, eye, up);
            break;
        case 'h':
            txVec3Cross(strafe, direction, up);
            txVec3Normalize(strafe, strafe);
            txVec3ScalarMul(strafe, strafe, CAMERA_SPEED);
            txVec3Sub(eye, eye, strafe);
            break;
        case 'l':
            txVec3Cross(strafe, direction, up);
            txVec3Normalize(strafe, strafe);
            txVec3ScalarMul(strafe, strafe, CAMERA_SPEED);
            txVec3Add(eye, eye, strafe);
            break;
        case 'f':
            flashLightEnabled = !flashLightEnabled;
            if (flashLightEnabled)
                txLight1f(0, TX_LIGHT_SPOT, TX_LIGHT_INTENSITY, 30.0f);
            else
                txLight1f(0, TX_LIGHT_SPOT, TX_LIGHT_INTENSITY, 0.0f);
            txComputeAttenuation(TX_LIGHT_SPOT, 0, 1.0f);
    }

    TXvec3 target;
    txVec3Add(target, eye, direction);
    txGenLookAtMatrix(txGetModelViewMatrix(), eye, target, up);

    txLight3fv(0, TX_LIGHT_SPOT, TX_LIGHT_DIRECTION, direction);
    txLight3fv(0, TX_LIGHT_SPOT, TX_LIGHT_POSITION, eye);

    // Set the light matrix as model-view matrix so that
    // lights work properly
    txCopyTransform(TX_LIGHT, TX_MODELVIEW);

    return false;
}

////////////////////////////////////////
/// example: first_person_demo
////////////////////////////////////////
/// This example shows how to implement
/// a basic first-person camera
///
/// CONTROLS:
/// Q to quit
/// W or K to move forward
/// S or J to move backward
/// A to turn left, D to turn right
/// Z to move up, C to move down
/// H to strafe left, L to strafe right
/// F to toggle flashlight
////////////////////////////////////////
int main(void)
{
    if (!(txInit() && txSetRenderWindow(stdscr, TX_BLOCK_MODE))) {
        fprintf(stderr, "ERROR: couldn't initialize TRex\n");
        return ERR_INIT;
    }
    WINDOW* input_win = createInputWindow();

    txClearColor3f(0.3f, 0.3f, 0.3f);
    txEnable(TX_DEPTH_TEST | TX_CULL_FACE);

    ////////////////////////////////////////
    ////////////// POINT LIGHT /////////////
    ////////////////////////////////////////
    txLight3f(0, TX_LIGHT_POINT, TX_LIGHT_DIFFUSE, POINT_LIGHT_COLOR_R,
                                                   POINT_LIGHT_COLOR_G,
                                                   POINT_LIGHT_COLOR_B);
    txLight3f(0, TX_LIGHT_POINT, TX_LIGHT_SPECULAR, POINT_LIGHT_COLOR_R,
                                                    POINT_LIGHT_COLOR_G,
                                                    POINT_LIGHT_COLOR_B);
    txLight1f(0, TX_LIGHT_POINT, TX_LIGHT_INTENSITY, 5.0f);
    txLight1f(0, TX_LIGHT_POINT, TX_LIGHT_RANGE, 1000.0f);
    txComputeAttenuation(TX_LIGHT_POINT, 0, 1.0f);

    ////////////////////////////////////////
    ///////////// FLASH LIGHT //////////////
    ////////////////////////////////////////
    txLight3f(0, TX_LIGHT_SPOT, TX_LIGHT_DIFFUSE, SPOT_LIGHT_COLOR_R,
                                                  SPOT_LIGHT_COLOR_G,
                                                  SPOT_LIGHT_COLOR_B);
    txLight3f(0, TX_LIGHT_SPOT, TX_LIGHT_SPECULAR, SPOT_LIGHT_COLOR_R,
                                                   SPOT_LIGHT_COLOR_G,
                                                   SPOT_LIGHT_COLOR_B);
    txLight1f(0, TX_LIGHT_SPOT, TX_LIGHT_CUTOFF, cosf(txToRadians(25.0f)));
    txLight1f(0, TX_LIGHT_SPOT, TX_LIGHT_RANGE, 25.0f);

    TXobjModel_t teapot;
    if (!txLoadObjModel("../obj/utah-teapot.obj", &teapot)) {
        txEnd();
        fprintf(stderr, "ERROR: couldn't load the utah-teapot\n");
        return ERR_LOAD;
    }

    float lightRotY = 0.0f;
    while (!processInput(input_win)) {
        txViewportMax();
        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);

        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        lightRotY += 0.01f;
        txLight3f(0, TX_LIGHT_POINT, TX_LIGHT_POSITION,
                  POINT_LIGHT_RADIUS * cosf(lightRotY),
                  POINT_LIGHT_HEIGHT,
                  POINT_LIGHT_RADIUS * sinf(lightRotY));

        txMatrixMode(TX_MODELVIEW);

        // Draw the floor
        txPushMatrix();
            txTranslate3f(0.0f, -1.0f, 0.0f);
            txScale3f(25.0f, 25.0f, 25.0f);

            txMaterial3f(TX_MATERIAL_AMBIENT, 1.0f, 1.0f, 1.0f);
            txMaterial3f(TX_MATERIAL_DIFFUSE, 1.0f, 1.0f, 1.0f);
            txMaterial3f(TX_MATERIAL_SPECULAR, 1.0f, 1.0f, 1.0f);
            txMaterial1f(TX_MATERIAL_SHININESS, 64.0f);

            txShadeModel(TX_SMOOTH);
            txDrawPlane();
        txPopMatrix();

        // Draw the light source as a point
        txPushMatrix();
            txTranslate3f(POINT_LIGHT_RADIUS * cosf(lightRotY),
                          POINT_LIGHT_HEIGHT,
                          POINT_LIGHT_RADIUS * sinf(lightRotY));

            txShadeModel(TX_UNLIT);
            txColor3f(POINT_LIGHT_COLOR_R,
                      POINT_LIGHT_COLOR_G,
                      POINT_LIGHT_COLOR_B);
            TXvec4 pointPos = { 0.0f, 0.0f, 0.0f, 1.0f };
            txDrawPoint(pointPos);
        txPopMatrix();

        // Draw the utah teapot
        txPushMatrix();
            txTranslate3f(0.0f, 1.0f, 0.0f);
            txRotate4f(lightRotY, 0.0f, 1.0f, 0.0f);
            txScale3f(2.0f, 2.0f, 2.0f);
            txShadeModel(TX_SMOOTH);
            txRenderObjModel(&teapot);
        txPopMatrix();

        txSwapBuffers();
    }

    txFreeObjModel(&teapot);
    delwin(input_win);
    txEnd();
    return SUCCESS;
}
