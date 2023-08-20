// Copyright (C) 2023 saccharineboi

#include "error.h"
#include "init.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

////////////////////////////////////////
void txOutputMessage(const TXappInfo_t* appInfo, enum TXmessageType type, const char* message, ...)
{
    assert(appInfo != NULL && message != NULL);

    if (appInfo->messageCallback) {
        va_list arg;
        va_start(arg, message);

        time_t now = time(NULL);
        struct tm* timeInfo = localtime(&now);

#define FORMATTED_MESSAGE_MAX_SIZE 250
        char formattedMessage[FORMATTED_MESSAGE_MAX_SIZE];
        int c = snprintf(formattedMessage, FORMATTED_MESSAGE_MAX_SIZE, "[%d::%d::%d %d::%d::%d] ", timeInfo->tm_mon,
                                                                                                   timeInfo->tm_mday,
                                                                                                   timeInfo->tm_year + 1900,
                                                                                                   timeInfo->tm_hour,
                                                                                                   timeInfo->tm_min,
                                                                                                   timeInfo->tm_sec);
        vsnprintf(formattedMessage + c, FORMATTED_MESSAGE_MAX_SIZE, message, arg);
        va_end(arg);

        appInfo->messageCallback(type, formattedMessage);
    }
}
