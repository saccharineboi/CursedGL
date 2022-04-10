// Copyright (C) 2022 saccharineboi

#include "error.h"

////////////////////////////////////////
static txMessageCallback messageCallback = NULL;

////////////////////////////////////////
void txSetMessageCallback(txMessageCallback callback)
{
    messageCallback = callback;
}

////////////////////////////////////////
void txOutputMessage(enum TXmessageType type, const char* message, ...)
{
    if (messageCallback) {
        va_list arg;
        va_start(arg, message);

#define FORMATTED_MESSAGE_MAX_SIZE 250
        char formattedMessage[FORMATTED_MESSAGE_MAX_SIZE];
        vsnprintf(formattedMessage, FORMATTED_MESSAGE_MAX_SIZE, message, arg);
        va_end(arg);

        messageCallback(type, formattedMessage);
    }
}
