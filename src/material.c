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

#include "material.h"

////////////////////////////////////////
static TXvec4 ambientColor;

////////////////////////////////////////
static TXvec4 diffuseColor;

////////////////////////////////////////
static TXvec4 specularColor;

////////////////////////////////////////
static float shininess;

////////////////////////////////////////
void txMaterial1f(enum TXmaterialScalarProperty prop, float value)
{
    switch (prop) {
        case TX_MATERIAL_SHININESS:
            shininess = value;
            break;
    }
}

////////////////////////////////////////
void txMaterial3f(enum TXmaterialArrayProperty prop, float r,
                                                     float g,
                                                     float b)
{
    switch (prop) {
        case TX_MATERIAL_AMBIENT:
            ambientColor[0] = r;
            ambientColor[1] = g;
            ambientColor[2] = b;
            break;
        case TX_MATERIAL_DIFFUSE:
            diffuseColor[0] = r;
            diffuseColor[1] = g;
            diffuseColor[2] = b;
            break;
        case TX_MATERIAL_SPECULAR:
            specularColor[0] = r;
            specularColor[1] = g;
            specularColor[2] = b;
            break;
    }
}

////////////////////////////////////////
void txMaterial3fv(enum TXmaterialArrayProperty prop, TXvec3 values)
{
    switch (prop) {
        case TX_MATERIAL_AMBIENT:
            txVec3Copy(ambientColor, values);
            break;
        case TX_MATERIAL_DIFFUSE:
            txVec3Copy(diffuseColor, values);
            break;
        case TX_MATERIAL_SPECULAR:
            txVec3Copy(specularColor, values);
            break;
    }
}

////////////////////////////////////////
void txMaterial4f(enum TXmaterialArrayProperty prop, float r,
                                                     float g,
                                                     float b,
                                                     float a)
{
    switch (prop) {
        case TX_MATERIAL_AMBIENT:
            ambientColor[0] = r;
            ambientColor[1] = g;
            ambientColor[2] = b;
            ambientColor[3] = a;
            break;
        case TX_MATERIAL_DIFFUSE:
            diffuseColor[0] = r;
            diffuseColor[1] = g;
            diffuseColor[2] = b;
            diffuseColor[3] = a;
            break;
        case TX_MATERIAL_SPECULAR:
            specularColor[0] = r;
            specularColor[1] = g;
            specularColor[2] = b;
            specularColor[3] = a;
            break;
    }
}

////////////////////////////////////////
void txMaterial4fv(enum TXmaterialArrayProperty prop, TXvec4 values)
{
    switch (prop) {
        case TX_MATERIAL_AMBIENT:
            txVec4Copy(ambientColor, values);
            break;
        case TX_MATERIAL_DIFFUSE:
            txVec4Copy(diffuseColor, values);
            break;
        case TX_MATERIAL_SPECULAR:
            txVec4Copy(specularColor, values);
            break;
    }
}

////////////////////////////////////////
float* txGetMaterial4fv(enum TXmaterialArrayProperty prop)
{
    switch (prop) {
        case TX_MATERIAL_AMBIENT:
            return ambientColor;
        case TX_MATERIAL_DIFFUSE:
            return diffuseColor;
        case TX_MATERIAL_SPECULAR:
            return specularColor;
    }
    return NULL;
}

////////////////////////////////////////
float txGetMaterial1f(enum TXmaterialScalarProperty prop)
{
    switch (prop) {
        case TX_MATERIAL_SHININESS:
            return shininess;
    }
    return 0.0f;
}
