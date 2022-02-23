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

#include "vec.h"

////////////////////////////////////////
/// Instead of maintaining multiple buffers,
/// we have a pixel object which stores all the
/// information we need to render it properly
/// onto the framebuffer
////////////////////////////////////////
struct TXpixel {
    TXvec4 color;
    float depth;
};

////////////////////////////////////////
typedef struct TXpixel TXpixel_t;

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
