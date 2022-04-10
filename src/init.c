// Copyright (C) 2022 saccharineboi

#include "init.h"
#include "framebuffer.h"
#include "lights.h"
#include "error.h"

#include <locale.h>

////////////////////////////////////////
static struct notcurses* ctx = NULL;

////////////////////////////////////////
bool txInit()
{
    if (!setlocale(LC_ALL, "")) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't set locale");
        return false;
    }

    if (!(ctx = notcurses_core_init(NULL, NULL))) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't initialize notcurses");
        return false;
    }

    if (!txInitFramebuffer(notcurses_stdplane(ctx))) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't initialize framebuffer");
        return false;
    }

    txOutputMessage(TX_INFO, "[CursedGL] txInit: successfully initialized CursedGL");
    return true;
}

////////////////////////////////////////
bool txEnd()
{
    if (!txFreeFramebuffer()) {
        txOutputMessage(TX_ERROR, "[CursedGL] txEnd: couldn't free the framebuffer(s)");
        return false;
    }
    return !notcurses_stop(ctx);
}

////////////////////////////////////////
struct notcurses* txGetContext()
{
    return ctx;
}
