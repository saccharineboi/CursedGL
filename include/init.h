// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include "error.h"

#include <notcurses/notcurses.h>

////////////////////////////////////////
struct TXappInfo
{
    struct notcurses* ctx;
    notcurses_options options;
    ncblitter_e blitter;
    txMessageCallback messageCallback;
};
typedef struct TXappInfo TXappInfo_t;

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
