// Copyright (C) 2022 saccharineboi

#include "error.h"

////////////////////////////////////////
static txErrorCallback errorCallback = NULL;

////////////////////////////////////////
static FILE* errorOutputFile = NULL;

////////////////////////////////////////
void txSetErrorCallback(txErrorCallback callback)
{
    errorCallback = callback;
}

////////////////////////////////////////
void txOutputError(const char* message)
{
    if (errorCallback && errorOutputFile) {
        errorCallback(errorOutputFile, message);
    }
}
