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
#include "common.h"

////////////////////////////////////////
enum TXmaterialArrayProperty {
    TX_MATERIAL_AMBIENT,
    TX_MATERIAL_DIFFUSE,
    TX_MATERIAL_SPECULAR
};

////////////////////////////////////////
enum TXmaterialScalarProperty {
    TX_MATERIAL_SHININESS
};

////////////////////////////////////////
void txMaterial1f(enum TXmaterialScalarProperty prop, float value);

////////////////////////////////////////
void txMaterial3f(enum TXmaterialArrayProperty prop, float r,
                                                     float g,
                                                     float b);

////////////////////////////////////////
void txMaterial3fv(enum TXmaterialArrayProperty prop, TXvec3 values);

////////////////////////////////////////
void txMaterial4f(enum TXmaterialArrayProperty prop, float r,
                                                     float g,
                                                     float b,
                                                     float a);

////////////////////////////////////////
void txMaterial4fv(enum TXmaterialArrayProperty prop, TXvec4 values);

////////////////////////////////////////
float* txGetMaterial4fv(enum TXmaterialArrayProperty prop);

////////////////////////////////////////
float txGetMaterial1f(enum TXmaterialScalarProperty prop);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
