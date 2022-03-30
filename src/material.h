// Copyright (C) 2022 saccharineboi

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
