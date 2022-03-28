// Copyright (C) 2022 saccharineboi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

    if (!notcurses_canchangecolor(ctx) || !notcurses_cantruecolor(ctx)) {
        fprintf(stderr, "ERROR: terminal not supported\n");
        notcurses_stop(ctx);
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
