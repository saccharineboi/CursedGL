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

#include "lights.h"
#include "transform.h"
#include "material.h"

#include <math.h>
#include <ncurses.h>
#include <unistd.h>

////////////////////////////////////////
static TXdirLight_t dirLights[TX_NUM_DIR_LIGHTS];

////////////////////////////////////////
static TXpointLight_t pointLights[TX_NUM_POINT_LIGHTS];

////////////////////////////////////////
static TXspotLight_t spotLights[TX_NUM_SPOT_LIGHTS];

////////////////////////////////////////
extern float* txGetLightMatrix();

////////////////////////////////////////
void txComputeDirLight(TXvec4 resultColor,
                       TXvec4 interpolatedNormal,
                       TXvec3 viewDir)
{
    float* ambientMaterial  = txGetMaterial4fv(TX_MATERIAL_AMBIENT);
    float* diffuseMaterial  = txGetMaterial4fv(TX_MATERIAL_DIFFUSE);
    float* specularMaterial = txGetMaterial4fv(TX_MATERIAL_SPECULAR);
    float shininess         = txGetMaterial1f(TX_MATERIAL_SHININESS);

    for (int i = 0; i < TX_NUM_DIR_LIGHTS; ++i) {
        if (txFloatEquals(dirLights[i].intensity, 0.0f))
            continue;

        ////////////////////////////////////////
        /// AMBIENT COLOR
        ////////////////////////////////////////
        TXvec3 ambientColor;
        txVec3Mul(ambientColor, dirLights[i].ambient, ambientMaterial);

        ////////////////////////////////////////
        /// DIFFUSE COLOR
        ////////////////////////////////////////
        TXvec3 lightDir;
        txMulMat4Vec3(lightDir, txGetLightMatrix(), dirLights[i].direction, 0.0f);
        txVec3Negate(lightDir, lightDir);
        txVec3Normalize(lightDir, lightDir);

        float diff = fmaxf(txVec3Dot(lightDir, interpolatedNormal), 0.0f);
        TXvec3 diffuseColor;
        txVec3Mul(diffuseColor, dirLights[i].diffuse, diffuseMaterial);
        txVec3ScalarMul(diffuseColor, diffuseColor, diff);

        ////////////////////////////////////////
        /// SPECULAR COLOR
        ////////////////////////////////////////
        TXvec3 halfwayDir;
        txVec3Add(halfwayDir, viewDir, lightDir);
        txVec3Normalize(halfwayDir, halfwayDir);

        float spec = powf(fmaxf(txVec3Dot(halfwayDir, interpolatedNormal), 0.0f), shininess);
        TXvec3 specularColor;
        txVec3Mul(specularColor, dirLights[i].specular, specularMaterial);
        txVec3ScalarMul(specularColor, specularColor, spec);

        ////////////////////////////////////////
        /// RESULTING COLOR
        ////////////////////////////////////////
        TXvec3 totalColor;
        txVec3Add(totalColor, diffuseColor, specularColor);
        txVec3ScalarMul(totalColor, totalColor, dirLights[i].intensity);
        txVec3Add(totalColor, totalColor, ambientColor);
        txVec3Add(resultColor, resultColor, totalColor);
    }

    txVec4Clamp(resultColor, resultColor, 0.0f, 1.0f);
}

////////////////////////////////////////
void txComputePointLight(TXvec4 resultColor,
                         TXvec4 interpolatedNormal,
                         TXvec3 interpolatedPosition,
                         TXvec3 viewDir)
{
    float* diffuseMaterial  = txGetMaterial4fv(TX_MATERIAL_DIFFUSE);
    float* specularMaterial = txGetMaterial4fv(TX_MATERIAL_SPECULAR);
    float shininess         = txGetMaterial1f(TX_MATERIAL_SHININESS);
    float* lightMatrix      = txGetLightMatrix();

    for (int i = 0; i < TX_NUM_POINT_LIGHTS; ++i) {
        if (txFloatEquals(pointLights[i].intensity, 0.0f))
            continue;

        ////////////////////////////////////////
        /// ATTENUATION
        ////////////////////////////////////////
        TXvec3 pointLightPosition;
        txMulMat4Vec3(pointLightPosition, lightMatrix, pointLights[i].position, 1.0f);

        float distance = txVec3Dist(pointLightPosition, interpolatedPosition);
        float den = (pointLights[i].constantAttenuation +
                     distance * pointLights[i].linearAttenuation +
                     distance * distance * pointLights[i].quadraticAttenuation);

        float attenuation;
        if (!txFloatEquals(den, 0.0f))
            attenuation = 1.0f / den;
        else
            continue;

        ////////////////////////////////////////
        /// DIFFUSE COLOR
        ////////////////////////////////////////
        TXvec3 direction;
        txVec3Sub(direction, interpolatedPosition, pointLightPosition);
        txVec3Negate(direction, direction);
        txVec3Normalize(direction, direction);

        float diff = fmaxf(txVec3Dot(direction, interpolatedNormal), 0.0f);
        TXvec3 diffuseColor;
        txVec3Mul(diffuseColor, diffuseMaterial, pointLights[i].diffuse);
        txVec3ScalarMul(diffuseColor, diffuseColor, diff);

        ////////////////////////////////////////
        /// SPECULAR COLOR
        ////////////////////////////////////////
        TXvec3 halfwayDir;
        txVec3Add(halfwayDir, viewDir, direction);
        txVec3Normalize(halfwayDir, halfwayDir);

        float spec = powf(fmaxf(txVec3Dot(interpolatedNormal, halfwayDir), 0.0f), shininess);
        TXvec3 specularColor;
        txVec3Mul(specularColor, pointLights[i].specular, specularMaterial);
        txVec3ScalarMul(specularColor, specularColor, spec);

        ////////////////////////////////////////
        /// RESULTING COLOR
        ////////////////////////////////////////
        TXvec3 totalColor;
        txVec3Add(totalColor, diffuseColor, specularColor);
        txVec3ScalarMul(totalColor, totalColor, pointLights[i].intensity * attenuation);
        txVec3Add(resultColor, resultColor, totalColor);
    }

    txVec4Clamp(resultColor, resultColor, 0.0f, 1.0f);
}

////////////////////////////////////////
void txComputeSpotLight(TXvec4 resultColor,
                        TXvec4 interpolatedNormal,
                        TXvec3 interpolatedPosition,
                        TXvec3 viewDir)
{
    float* diffuseMaterial  = txGetMaterial4fv(TX_MATERIAL_DIFFUSE);
    float* specularMaterial = txGetMaterial4fv(TX_MATERIAL_SPECULAR);
    float shininess         = txGetMaterial1f(TX_MATERIAL_SHININESS);
    float* lightMatrix      = txGetLightMatrix();

    for (int i = 0; i < TX_NUM_SPOT_LIGHTS; ++i) {
        if (txFloatEquals(spotLights[i].intensity, 0.0f))
            continue;

        ////////////////////////////////////////
        /// CUTOFF
        ////////////////////////////////////////
        TXvec3 spotLightPosition;
        txMulMat4Vec3(spotLightPosition, lightMatrix, spotLights[i].position, 1.0f);

        TXvec3 direction;
        txVec3Sub(direction, interpolatedPosition, spotLightPosition);
        txVec3Normalize(direction, direction);
        txVec3Negate(direction, direction);

        TXvec3 lightDirection;
        txMulMat4Vec3(lightDirection, lightMatrix, spotLights[i].direction, 0.0f);
        txVec3Negate(lightDirection, lightDirection);
        txVec3Normalize(lightDirection, lightDirection);

        float theta = txVec3Dot(direction, lightDirection);
        if (theta > spotLights[i].cutoff) {

            ////////////////////////////////////////
            /// ATTENUATION
            ////////////////////////////////////////
            float distance = txVec3Dist(spotLights[i].position, interpolatedPosition);
            float den = (spotLights[i].constantAttenuation +
                         distance * spotLights[i].linearAttenuation +
                         distance * distance * spotLights[i].quadraticAttenuation);

            float attenuation;
            if (!txFloatEquals(den, 0.0f))
                attenuation = 1.0f / den;
            else
                continue;

            ////////////////////////////////////////
            /// DIFFUSE COLOR
            ////////////////////////////////////////
            float diff = fmaxf(txVec3Dot(direction, interpolatedNormal), 0.0f);
            TXvec3 diffuseColor;
            txVec3Mul(diffuseColor, diffuseMaterial, spotLights[i].diffuse);
            txVec3ScalarMul(diffuseColor, diffuseColor, diff);

            ////////////////////////////////////////
            /// SPECULAR COLOR
            ////////////////////////////////////////
            TXvec3 halfwayDir;
            txVec3Add(halfwayDir, viewDir, direction);
            txVec3Normalize(halfwayDir, halfwayDir);

            float spec = powf(fmaxf(txVec3Dot(interpolatedNormal, halfwayDir), 0.0f), shininess);
            TXvec3 specularColor;
            txVec3Mul(specularColor, spotLights[i].specular, specularMaterial);
            txVec3ScalarMul(specularColor, specularColor, spec);

            ////////////////////////////////////////
            /// RESULTING COLOR
            ////////////////////////////////////////
            TXvec3 totalColor;
            txVec3Add(totalColor, diffuseColor, specularColor);
            txVec3ScalarMul(totalColor, totalColor, spotLights[i].intensity * attenuation);
            txVec3Add(resultColor, resultColor, totalColor);
        }
    }

    txVec4Clamp(resultColor, resultColor, 0.0f, 1.0f);
}

////////////////////////////////////////
void txComputeAttenuation(enum TXlightType lightType, int lightIndex, float constant)
{
    if (lightIndex < 0)
        return;

    float range;
    switch (lightType) {
        case TX_LIGHT_POINT:
            if (lightIndex < TX_NUM_POINT_LIGHTS) {
                pointLights[lightIndex].constantAttenuation = constant;
                range = pointLights[lightIndex].range;
                pointLights[lightIndex].linearAttenuation = 4.5f / range;
                pointLights[lightIndex].quadraticAttenuation = 75.0f / range;
            }
            break;
        case TX_LIGHT_SPOT:
            if (lightIndex < TX_NUM_SPOT_LIGHTS) {
                spotLights[lightIndex].constantAttenuation = constant;
                range = spotLights[lightIndex].range;
                spotLights[lightIndex].linearAttenuation = 4.5f / range;
                spotLights[lightIndex].quadraticAttenuation = 75.0f / range;
            }
            break;
        case TX_LIGHT_DIRECTIONAL:
            break;
    }
}

////////////////////////////////////////
void txLight1f(int lightIndex,
               enum TXlightType lightType,
               enum TXlightScalarProperty property,
               float s)
{
    switch (lightType) {
        case TX_LIGHT_DIRECTIONAL:
            if (lightIndex < 0 || lightIndex >= TX_NUM_DIR_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    dirLights[lightIndex].intensity = s;
                    break;
                case TX_LIGHT_RANGE:
                case TX_LIGHT_CONSTANT_ATTENUATION:
                case TX_LIGHT_LINEAR_ATTENUATION:
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                case TX_LIGHT_CUTOFF:
                    break;
            }
            break;
        case TX_LIGHT_POINT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_POINT_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    pointLights[lightIndex].intensity = s;
                    break;
                case TX_LIGHT_RANGE:
                    pointLights[lightIndex].range = s;
                    break;
                case TX_LIGHT_CONSTANT_ATTENUATION:
                    pointLights[lightIndex].constantAttenuation = s;
                    break;
                case TX_LIGHT_LINEAR_ATTENUATION:
                    pointLights[lightIndex].linearAttenuation = s;
                    break;
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                    pointLights[lightIndex].quadraticAttenuation = s;
                    break;
                case TX_LIGHT_CUTOFF:
                    break;
            }
            break;
        case TX_LIGHT_SPOT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_SPOT_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    spotLights[lightIndex].intensity = s;
                    break;
                case TX_LIGHT_RANGE:
                    spotLights[lightIndex].range = s;
                    break;
                case TX_LIGHT_CONSTANT_ATTENUATION:
                    spotLights[lightIndex].constantAttenuation = s;
                    break;
                case TX_LIGHT_LINEAR_ATTENUATION:
                    spotLights[lightIndex].linearAttenuation = s;
                    break;
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                    spotLights[lightIndex].quadraticAttenuation = s;
                    break;
                case TX_LIGHT_CUTOFF:
                    spotLights[lightIndex].cutoff = s;
                    break;
            }
            break;
    }
}

////////////////////////////////////////
void txLight3fv(int lightIndex,
                enum TXlightType lightType,
                enum TXlightArrayProperty property,
                TXvec3 values)
{
    switch (lightType) {
        case TX_LIGHT_DIRECTIONAL:
            if (lightIndex < 0 || lightIndex >= TX_NUM_DIR_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_AMBIENT:
                    txVec3Copy(dirLights[lightIndex].ambient, values);
                    break;
                case TX_LIGHT_DIFFUSE:
                    txVec3Copy(dirLights[lightIndex].diffuse, values);
                    break;
                case TX_LIGHT_SPECULAR:
                    txVec3Copy(dirLights[lightIndex].specular, values);
                    break;
                case TX_LIGHT_DIRECTION:
                    txVec3Copy(dirLights[lightIndex].direction, values);
                    break;
                case TX_LIGHT_POSITION:
                    break;
            }
            break;
        case TX_LIGHT_POINT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_POINT_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_DIFFUSE:
                    txVec3Copy(pointLights[lightIndex].diffuse, values);
                    break;
                case TX_LIGHT_SPECULAR:
                    txVec3Copy(pointLights[lightIndex].specular, values);
                    break;
                case TX_LIGHT_POSITION:
                    txVec3Copy(pointLights[lightIndex].position, values);
                    break;
                case TX_LIGHT_AMBIENT:
                case TX_LIGHT_DIRECTION:
                    break;
            }
            break;
        case TX_LIGHT_SPOT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_SPOT_LIGHTS)
                return;
            switch (property) {
                case TX_LIGHT_DIFFUSE:
                    txVec3Copy(spotLights[lightIndex].diffuse, values);
                    break;
                case TX_LIGHT_SPECULAR:
                    txVec3Copy(spotLights[lightIndex].specular, values);
                    break;
                case TX_LIGHT_DIRECTION:
                    txVec3Copy(spotLights[lightIndex].direction, values);
                    break;
                case TX_LIGHT_POSITION:
                    txVec3Copy(spotLights[lightIndex].position, values);
                    break;
                case TX_LIGHT_AMBIENT:
                    break;
            }
            break;
    }
}

////////////////////////////////////////
void txLight3f(int lightIndex,
               enum TXlightType lightType,
               enum TXlightArrayProperty property,
               float x, float y, float z)
{
    TXvec3 values = { x, y, z };
    txLight3fv(lightIndex, lightType, property, values);
}

////////////////////////////////////////
float txLightGet1f(int lightIndex,
                   enum TXlightType lightType,
                   enum TXlightScalarProperty property)
{
    switch (lightType) {
        case TX_LIGHT_DIRECTIONAL:
            if (lightIndex < 0 || lightIndex >= 0)
                return 0.0f;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    return dirLights[lightIndex].intensity;
                case TX_LIGHT_RANGE:
                case TX_LIGHT_CONSTANT_ATTENUATION:
                case TX_LIGHT_LINEAR_ATTENUATION:
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                case TX_LIGHT_CUTOFF:
                    return 0.0f;
            }
            break;
        case TX_LIGHT_POINT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_POINT_LIGHTS)
                return 0.0f;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    return pointLights[lightIndex].intensity;
                case TX_LIGHT_RANGE:
                    return pointLights[lightIndex].range;
                case TX_LIGHT_CONSTANT_ATTENUATION:
                    return pointLights[lightIndex].constantAttenuation;
                case TX_LIGHT_LINEAR_ATTENUATION:
                    return pointLights[lightIndex].linearAttenuation;
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                    return pointLights[lightIndex].quadraticAttenuation;
                case TX_LIGHT_CUTOFF:
                    return 0.0f;
            }
            break;
        case TX_LIGHT_SPOT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_SPOT_LIGHTS)
                return 0.0f;
            switch (property) {
                case TX_LIGHT_INTENSITY:
                    return spotLights[lightIndex].intensity;
                case TX_LIGHT_RANGE:
                    return spotLights[lightIndex].range;
                case TX_LIGHT_CONSTANT_ATTENUATION:
                    return spotLights[lightIndex].constantAttenuation;
                case TX_LIGHT_LINEAR_ATTENUATION:
                    return spotLights[lightIndex].linearAttenuation;
                case TX_LIGHT_QUADRATIC_ATTENUATION:
                    return spotLights[lightIndex].quadraticAttenuation;
                case TX_LIGHT_CUTOFF:
                    return spotLights[lightIndex].cutoff;
            }
            break;
    }
    return 0.0f;
}

////////////////////////////////////////
float* txLightGet3fv(int lightIndex,
                     enum TXlightType lightType,
                     enum TXlightArrayProperty property)
{
    switch (lightType) {
        case TX_LIGHT_DIRECTIONAL:
            if (lightIndex < 0 || lightIndex >= TX_NUM_DIR_LIGHTS)
                return NULL;
            switch (property) {
                case TX_LIGHT_AMBIENT:
                    return dirLights[lightIndex].ambient;
                case TX_LIGHT_DIFFUSE:
                    return dirLights[lightIndex].diffuse;
                case TX_LIGHT_SPECULAR:
                    return dirLights[lightIndex].specular;
                case TX_LIGHT_DIRECTION:
                    return dirLights[lightIndex].direction;
                case TX_LIGHT_POSITION:
                    return NULL;
            }
            break;
        case TX_LIGHT_POINT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_POINT_LIGHTS)
                return NULL;
            switch (property) {
                case TX_LIGHT_DIFFUSE:
                    return pointLights[lightIndex].diffuse;
                case TX_LIGHT_SPECULAR:
                    return pointLights[lightIndex].specular;
                case TX_LIGHT_POSITION:
                    return pointLights[lightIndex].position;
                case TX_LIGHT_AMBIENT:
                case TX_LIGHT_DIRECTION:
                    return NULL;
            }
            break;
        case TX_LIGHT_SPOT:
            if (lightIndex < 0 || lightIndex >= TX_NUM_SPOT_LIGHTS)
                return NULL;
            switch (property) {
                case TX_LIGHT_DIFFUSE:
                    return spotLights[lightIndex].diffuse;
                case TX_LIGHT_SPECULAR:
                    return spotLights[lightIndex].specular;
                case TX_LIGHT_DIRECTION:
                    return spotLights[lightIndex].direction;
                case TX_LIGHT_POSITION:
                    return spotLights[lightIndex].position;
                case TX_LIGHT_AMBIENT:
                    return NULL;
            }
            break;
    }
    return NULL;
}
