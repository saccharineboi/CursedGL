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
#include "vec.h"
#include "mat4.h"

////////////////////////////////////////
/// Feel free to change these numbers,
/// though you should know that the more
/// lights your application has, more
/// CPU-hungry it will become
////////////////////////////////////////
#define TX_NUM_DIR_LIGHTS   2
#define TX_NUM_POINT_LIGHTS 2
#define TX_NUM_SPOT_LIGHTS  2

////////////////////////////////////////
/// Types of light supported by TRex.
/// Note that point and spot lights do
/// not contribute to the ambient coloring
/// of objects in the scene
////////////////////////////////////////
enum TXlightType {
    TX_LIGHT_DIRECTIONAL,
    TX_LIGHT_POINT,
    TX_LIGHT_SPOT,
    // TX_LIGHT_AREA // one can dream ...
};

////////////////////////////////////////
enum TXlightArrayProperty {
    TX_LIGHT_AMBIENT,
    TX_LIGHT_DIFFUSE,
    TX_LIGHT_SPECULAR,
    TX_LIGHT_DIRECTION,
    TX_LIGHT_POSITION
};

////////////////////////////////////////
enum TXlightScalarProperty {
    TX_LIGHT_INTENSITY,
    TX_LIGHT_RANGE,
    TX_LIGHT_CONSTANT_ATTENUATION,
    TX_LIGHT_LINEAR_ATTENUATION,
    TX_LIGHT_QUADRATIC_ATTENUATION,
    TX_LIGHT_CUTOFF
};

////////////////////////////////////////
struct TXdirLight {
    TXvec3 ambient;
    TXvec3 diffuse;
    TXvec3 specular;
    TXvec3 direction;
    float intensity;
};
typedef struct TXdirLight TXdirLight_t;

////////////////////////////////////////
struct TXpointLight {
    TXvec3 diffuse;
    TXvec3 specular;
    TXvec3 position;
    float intensity;
    float range;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
typedef struct TXpointLight TXpointLight_t;

////////////////////////////////////////
struct TXspotLight {
    TXvec3 diffuse;
    TXvec3 specular;
    TXvec3 position;
    TXvec3 direction;
    float intensity;
    float range;
    float cutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
typedef struct TXspotLight TXspotLight_t;

////////////////////////////////////////
void txComputeDirLight(TXvec4 resultColor,
                       TXvec4 interpolatedNormal,
                       TXvec3 viewDir);

////////////////////////////////////////
void txComputePointLight(TXvec4 resultColor,
                         TXvec4 interpolatedNormal,
                         TXvec4 interpolatedPosition,
                         TXvec3 viewDir);

////////////////////////////////////////
void txComputeSpotLight(TXvec4 resultColor,
                        TXvec4 interpolatedNormal,
                        TXvec4 interpolatedPosition,
                        TXvec3 viewDir);

////////////////////////////////////////
/// Approximates constant attenuation,
/// linear attenuation, and quadratic
/// attenuation based on range property
///
/// See: https://wiki.ogre3d.org/Light+Attenuation+Shortcut
////////////////////////////////////////
void txComputeAttenuation(enum TXlightType lightType, int lightIndex, float constant);

////////////////////////////////////////
void txLight1f(int lightIndex,
               enum TXlightType lightType,
               enum TXlightScalarProperty property,
               float scalar);

////////////////////////////////////////
void txLight3f(int lightIndex,
               enum TXlightType lightType,
               enum TXlightArrayProperty property,
               float x, float y, float z);

////////////////////////////////////////
void txLight3fv(int lightIndex,
                enum TXlightType lightType,
                enum TXlightArrayProperty property,
                TXvec3 values);

////////////////////////////////////////
float txLightGet1f(int lightIndex,
                   enum TXlightType lightType,
                   enum TXlightScalarProperty property);

////////////////////////////////////////
float* txLightGet3fv(int lightIndex,
                     enum TXlightType lightType,
                     enum TXlightArrayProperty property);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
