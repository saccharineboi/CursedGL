// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "vec.h"

////////////////////////////////////////
/// Instead of maintaining multiple buffers,
/// we have a pixel object which stores all the
/// information we need to render it properly
/// onto the framebuffer
////////////////////////////////////////
struct TXpixel {
    TXvec4 color;
    float depth;
};

////////////////////////////////////////
typedef struct TXpixel TXpixel_t;

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
