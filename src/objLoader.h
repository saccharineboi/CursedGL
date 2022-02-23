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

///  NOTE: DO NOT USE THIS OBJ LOADER FOR OBJ FILES
///  OTHER THAN THOSE IN THE "obj" FOLDER LOCATED IN
///  THE ROOT FOLDER OF THE PROJECT.
///
///  If you wish to load your own models check out https://assimp.org

////////////////////////////////////////
struct TXobjModel {
    float* positions;
    float* normals;
    float* texCoords;

    unsigned* indices;
    unsigned numIndices;
};

////////////////////////////////////////
typedef struct TXobjModel TXobjModel_t;

////////////////////////////////////////
/// Loads the obj model from a given path,
/// and stores it in the model struct.
/// Returns true if successful, false otherwise
////////////////////////////////////////
bool txLoadObjModel(const char* filePath, TXobjModel_t* model);

////////////////////////////////////////
void txRenderObjModel(TXobjModel_t* model);

////////////////////////////////////////
void txFreeObjModel(TXobjModel_t* model);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
