<img src=".github/gif_files/cursedgl_animated.GIF" width="100%" height="auto">

*Made with C99* <br/>
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7648729701844ab59e39fc0794d96701)](https://www.codacy.com/gh/saccharineboi/CursedGL/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=saccharineboi/CursedGL&amp;utm_campaign=Badge_Grade)
<a href="#"><img src="https://img.shields.io/badge/Maintained%3F-yes-green.svg"/></a>
<a href="#"><img src="https://img.shields.io/github/issues/saccharineboi/CursedGL.svg"/></a>
<a href="#"><img src="https://img.shields.io/github/issues-closed/saccharineboi/CursedGL.svg"/></a>
<a href="#"><img src="https://img.shields.io/github/issues-pr/saccharineboi/CursedGL.svg"/></a>
<a href="#"><img src="https://img.shields.io/github/issues-pr-closed/saccharineboi/CursedGL.svg"/></a>

## Description

**CursedGL (Cursed Graphics Library) is a notcurses-based software rasterizer inspired by OpenGL 1.X that renders directly to the terminal**

## Usage

```c

#include <cursedgl.h>

// File that will contain CursedGL text output
#define OUTPUT_FILE "usage.txt"

// Quit rendering if Q is pressed
static bool processInput()
{
    struct ncinput input;
    notcurses_get_nblock(txGetContext(), &input);

    if (input.id == 'q')
        return true;
    return false;
}

// Callback used for getting information from CursedGL
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

// Spinning triangle
int main(void)
{
    // Set callback to listen to CursedGL's text output
    txSetMessageCallback(messageCallback);

    // Initialize CursedGL
    if (!txInit())
        return -1;

    // Set the color the framebuffer will be cleared with
    txClearColor3f(0.2f, 0.3f, 0.3f);

    // CursedGL uses right-handed coordinate-system
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
                        {  1.0f,  0.0f, 0.0f, 1.0f } };
        TXvec4 v1[] = { {  1.0f, -1.0f, 0.0f, 1.0f },
                        {  0.0f,  1.0f, 0.0f, 1.0f } };
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

````

## Output

https://user-images.githubusercontent.com/95090318/160808050-00e4c22b-03b3-4f51-9ebc-5eebd17b12ec.mp4

## Features

  - Supports points, lines, line-loops, line-strips, triangles, triangle-fans, triangle-strips, and quads
  - In addition to the above some basic shapes like cubes and planes are provided out-of-the-box
  - Face Culling
  - Triangle Clipping
  - Perspective-Correct Vertex-Attribute Interpolation
  - Early Depth Testing
  - 3 Shading Modes:
    - Unlit
    - Flat
    - Smooth
  - Built-in Math Library
  - Right-handed Coordinate System
  - Multi-threaded rendering
  - Double-buffering
  - API similar to OpenGL's Immediate Mode Interface
  - 3 Supported Light Types:
    - Directional
    - Point
    - Spot
  - Matrix Stacks

## Known Bugs And Issues

  - No texture support *yet*
  - More sophisticated multi-threading system would be better
  - Math library needs SIMD implementation
  - No stencil buffer
  - No blending
  - Code needs testing

## Building

### Linux
    
Make sure to have GCC, notcurses, and cmake installed.

```
$ git clone https://github.com/saccharineboi/CursedGL.git
$ cd CursedGL
$ mkdir build && cd build
$ cmake ..
$ make
```
    
### Windows 10, 11

Windows 10 and 11 come pre-installed with Linux kernel out-of-the-box.
    
You need to enable WSL2 and install Ubuntu from the Windows Store.
    
[This video will be helpful.](https://youtu.be/_fntjriRe48)
    
After you've got everything working, follow the instructions for the linux users above.
    
## Learning
*Check the examples/ folder for code samples*
    
*Check the comments in the source files under src/ folder*

## Contact

Grab my public key [here](https://flowcrypt.com/pub/saccharineboi@gmail.com?show=pubkey) and send me an email.
    
PGP fingerprint: 693E AD50 5142 9D3F 74D9  F415 869B 4A61 D04F AD71

## Similar Projects
    
  - [minirender](https://github.com/aslze/minirender) by *asize* : A simple software rasterizer for 3D meshes
    
## License

Copyright (C) 2022 saccharineboi

CursedGL is dual-licensed under GPLv3 and LGPLv3. Choose whichever you like.
