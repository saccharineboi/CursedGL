// Copyright (C) 2022 saccharineboi

#include "init.h"
#include "framebuffer.h"
#include "lights.h"

#include <locale.h>

////////////////////////////////////////
static struct notcurses* ctx = NULL;

////////////////////////////////////////
bool txInit()
{
    if (!setlocale(LC_ALL, "")) {
        fprintf(stderr, "ERROR: couldn't set locale\n");
        return false;
    }

    if (!(ctx = notcurses_core_init(NULL, NULL))) {
        fprintf(stderr, "ERROR: couldn't initialize notcurses\n");
        return false;
    }

    if (!txInitFramebuffer(notcurses_stdplane(ctx))) {
        notcurses_stop(ctx);
        fprintf(stderr, "ERROR: couldn't initialize framebuffer\n");
        return false;
    }

    return true;
}

////////////////////////////////////////
bool txEnd()
{
    if (!txFreeFramebuffer())
        return false;
    return !notcurses_stop(ctx);
}

////////////////////////////////////////
struct notcurses* txGetContext()
{
    return ctx;
}
