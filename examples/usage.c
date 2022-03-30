// Copyright (C) 2022 saccharineboi

#include <cursedgl.h>

// Quit rendering if Q is pressed
static bool processInput()
{
    struct ncinput input;
    notcurses_get_nblock(txGetContext(), &input);

    if (input.id == 'q')
        return true;
    return false;
}

// Spinning triangle
int main(void)
{
    // Initialize CursedGL
    if (!txInit()) {
        fprintf(stderr, "ERROR: couldn't initialize CursedGL\n");
        return -1;
    }

    // Set the color the framebuffer will be cleared with
    txClearColor3f(0.2f, 0.3f, 0.3f);

    // TRex uses right-handed coordinate-system
    txTranslate3f(0.0f, 0.0f, -2.0f);

    // Render loop
    while (!processInput()) {

        // Clear the framebuffer
        txClear(TX_COLOR_BIT | TX_DEPTH_BIT);

        // Occupy the whole terminal area
        txViewportMax();

        // Use perspective projection
        txPerspective(TX_PI / 2.0f,
                      txGetFramebufferAspectRatio(),
                      0.1f,
                      100.0f);

        // Specify the vertex data for our triangle
        TXvec4 v0[] = { { -1.0f, -1.0f, 0.0f, 1.0f },
                      {    1.0f,  0.0f, 0.0f, 1.0f } };
        TXvec4 v1[] = { {  1.0f, -1.0f, 0.0f, 1.0f },
                      {    0.0f,  1.0f, 0.0f, 1.0f } };
        TXvec4 v2[] = { {  0.0f,  1.0f, 0.0f, 1.0f },
                        {  0.0f,  0.0f, 1.0f, 1.0f } };

        // Rotate the triangle continuously around y-axis
        txRotate4f(0.01f, 0.0f, 1.0f, 0.0f);

        // Draw the triangle with one of the predefined
        // vertex-attribute configurations (here we use
        // TX_POSITION_COLOR, for more see enum TXvertexInfo
        // in rasterizer.h)
        txDrawTriangle(v0, v1, v2, TX_POSITION_COLOR);

        // Swap the front and back framebuffers
        txSwapBuffers();
    }

    // Free memory used for rendering
    txEnd();

    // Success
    return 0;
}
