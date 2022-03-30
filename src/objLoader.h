// Copyright (C) 2022 saccharineboi

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
