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
#include "colors.h"
#include "framebuffer.h"
#include "lights.h"

////////////////////////////////////////
bool txInit(int colorMode)
{
    if (!initscr()) {
        fprintf(stderr, "ERROR: failed to initialize ncurses\n");
        return false;
    }

    if (start_color() == ERR) {
        endwin();
        fprintf(stderr, "ERROR: start_color() failed\n");
        return false;
    }

    if (!has_colors() || !can_change_color()) {
        endwin();
        fprintf(stderr, "ERROR: colors not supported\n");
        return false;
    }
    txInitColors(colorMode);

    if (!txInitFramebuffer()) {
        endwin();
        fprintf(stderr, "ERROR: failed to initialize framebuffer\n");
        return false;
    }

    return true;
}

////////////////////////////////////////
bool txEnd()
{
    if (!txFreeFramebuffer())
        return false;
    endwin();
    return true;
}
