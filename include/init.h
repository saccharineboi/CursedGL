// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include <notcurses/notcurses.h>

////////////////////////////////////////
enum TXmessageType { TX_INFO, TX_WARNING, TX_ERROR };

////////////////////////////////////////
typedef void (*txMessageCallback) (enum TXmessageType type, const char* message);

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
