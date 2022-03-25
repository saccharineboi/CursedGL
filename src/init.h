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

#pragma once

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

////////////////////////////////////////
/// Tries to initialize CursedGL, returns true
/// on success, false otherwise
////////////////////////////////////////
bool txInit();

////////////////////////////////////////
/// Tries to free resources owned by CursedGL,
/// returns true on success, false otherwise
////////////////////////////////////////
bool txEnd();

////////////////////////////////////////
/// Sets the window CursedGL will be rendering
/// onto, and the color mode for the rendering,
/// returns true on success, false otherwise
////////////////////////////////////////
bool txSetRenderWindow(WINDOW* renderWindow, int colorMode);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
