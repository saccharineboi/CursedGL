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

#include "common.h"
#include <ncurses.h>

////////////////////////////////////////
/// For color-pairs, ncurses reserves
/// 0 as a special pair. Hence
/// TX_COLORPAIR_START must be >= 1
////////////////////////////////////////
#define TX_COLORPAIR_START 1

////////////////////////////////////////
/// Block mode produces PS1-style retro graphics*,
/// whereas ASCII mode produces the familiar
/// ASCII graphics on the terminal**
///
/// * Note that technically CursedGL doesn't produce
/// PS1-style graphics since CursedGL performs
/// perspective-correct vertex-attribute
/// interpolation. Nevertheless, the resulting
/// graphics look blocky and pixelated, similar to
/// PS1 graphics
///
/// ** See the implementation of txInitColors to
/// find out how CursedGL does this
////////////////////////////////////////
enum TXcolorMode { TX_BLOCK_MODE,
                   TX_ASCII_MODE };

////////////////////////////////////////
void txInitColors(int mode);

////////////////////////////////////////
/// Number of gradients of each color.
/// These numbers determine
/// the precision of each color component.
////////////////////////////////////////
#define TX_NUM_R 6.0f
#define TX_NUM_G 6.0f
#define TX_NUM_B 6.0f

////////////////////////////////////////
/// Returns the color pair approximating the color
/// given as r, g, b values, each of which ranges
/// from 0.0f to 1.0f
////////////////////////////////////////
TX_FORCE_INLINE int txGetColorPair(float r, float g, float b)
{
    int r_i = (int)(roundf(r * (TX_NUM_R - 1.0f)) * TX_NUM_G * TX_NUM_B);
    int g_i = (int)(roundf(g * (TX_NUM_G - 1.0f)) * TX_NUM_B);
    int b_i = (int)(roundf(b * (TX_NUM_B - 1.0f)));

    return TX_COLORPAIR_START + b_i + g_i + r_i;
}

////////////////////////////////////////
/// Sets the ncurses color best approximating the color
/// given as r, g, b values, each of which ranges
/// from 0.0f to 1.0f
////////////////////////////////////////
TX_FORCE_INLINE void txSetColor(float r, float g, float b)
{
    int pair = txGetColorPair(r, g, b);
    extern WINDOW* txGetRenderWindow();
    wattron(txGetRenderWindow(), COLOR_PAIR(pair));
}

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
