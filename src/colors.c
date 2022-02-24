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

#include "colors.h"

#include <ncurses.h>

////////////////////////////////////////
/// Initializes colors based on whether
/// the user has requested ASCII mode
/// or BLOCK mode
////////////////////////////////////////
void txInitColors(int mode)
{
#define R_OFFSET (1000.0f / (TX_NUM_R - 1))
#define G_OFFSET (1000.0f / (TX_NUM_G - 1))
#define B_OFFSET (1000.0f / (TX_NUM_B - 1))

    short color_ctr = 0;
    short pair_ctr  = TX_COLORPAIR_START;

    for (int r = 0; r < TX_NUM_R; ++r) {
        for (int g = 0; g < TX_NUM_G; ++g) {
            for (int b = 0; b < TX_NUM_B; ++b) {
                init_color(color_ctr, (short)((float)r * R_OFFSET),
                                      (short)((float)g * G_OFFSET),
                                      (short)((float)b * B_OFFSET));

                // To create the BLOCK mode all we do is simply set
                // the background color to be the same as the
                // foreground color. For ASCII mode we set the
                // background color to 0: i.e. black, making the
                // ASCII letters stand out
                if (mode == TX_BLOCK_MODE)
                    init_pair(pair_ctr, color_ctr, color_ctr);
                else if (mode == TX_ASCII_MODE)
                    init_pair(pair_ctr, color_ctr, 0);

                ++pair_ctr;
                ++color_ctr;
            }
        }
    }
}
