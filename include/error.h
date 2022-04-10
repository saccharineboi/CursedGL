// Copyright (C) 2022 saccharineboi

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>

////////////////////////////////////////
enum TXmessageType { TX_INFO, TX_WARNING, TX_ERROR };

////////////////////////////////////////
typedef void (*txMessageCallback) (enum TXmessageType type, const char* message);

////////////////////////////////////////
void txSetMessageCallback(txMessageCallback callback);

////////////////////////////////////////
void txOutputMessage(enum TXmessageType type, const char* message, ...);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////