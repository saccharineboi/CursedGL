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

#include "shapes.h"
#include "vec.h"
#include "rasterizer.h"

#include <math.h>

////////////////////////////////////////
/// NOTE:
/// Make sure to set the w value of
/// directional vectors to 0.0f,
/// positional vectors to 1.0f.
/// This is how TRex differentiates between
/// vectors that represent direction vs
/// vectors that represent position
////////////////////////////////////////

////////////////////////////////////////
void txDrawPlane()
{
    static TXvec4 v0[] = { { -0.5f, 0.0f,  0.5f, 1.0f },
                           {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 v1[] = { {  0.5f, 0.0f,  0.5f, 1.0f },
                           {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 v2[] = { {  0.5f, 0.0f, -0.5f, 1.0f },
                           {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 v3[] = { { -0.5f, 0.0f, -0.5f, 1.0f },
                           {  0.0f, 1.0f,  0.0f, 0.0f } };

    txDrawQuad(v0, v1, v2, v3, TX_POSITION_NORMAL);
}

////////////////////////////////////////
void txDrawCube()
{
    // front face
    static TXvec4 front_v0[] = { { -0.5f, -0.5f, 0.5f, 1.0f },
                                 {  0.0f,  0.0f, 1.0f, 0.0f } };
    static TXvec4 front_v1[] = { {  0.5f, -0.5f, 0.5f, 1.0f },
                                 {  0.0f,  0.0f, 1.0f, 0.0f } };
    static TXvec4 front_v2[] = { {  0.5f,  0.5f, 0.5f, 1.0f },
                                 {  0.0f,  0.0f, 1.0f, 0.0f } };
    static TXvec4 front_v3[] = { { -0.5f,  0.5f, 0.5f, 1.0f },
                                 {  0.0f,  0.0f, 1.0f, 0.0f } };

    // left face
    static TXvec4 left_v0[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                { -1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 left_v1[] = { { -0.5f, -0.5f,  0.5f, 1.0f },
                                { -1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 left_v2[] = { { -0.5f,  0.5f,  0.5f, 1.0f },
                                { -1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 left_v3[] = { { -0.5f,  0.5f, -0.5f, 1.0f },
                                { -1.0f,  0.0f,  0.0f, 0.0f } };

    // right face
    static TXvec4 right_v0[] = { { 0.5f, -0.5f,  0.5f, 1.0f },
                                 { 1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 right_v1[] = { { 0.5f, -0.5f, -0.5f, 1.0f },
                                 { 1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 right_v2[] = { { 0.5f,  0.5f, -0.5f, 1.0f },
                                 { 1.0f,  0.0f,  0.0f, 0.0f } };
    static TXvec4 right_v3[] = { { 0.5f,  0.5f,  0.5f, 1.0f },
                                 { 1.0f,  0.0f,  0.0f, 0.0f } };

    // back face
    static TXvec4 back_v0[] = { {  0.5f, -0.5f, -0.5f, 1.0f },
                                {  0.0f,  0.0f, -1.0f, 0.0f } };
    static TXvec4 back_v1[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                {  0.0f,  0.0f, -1.0f, 0.0f } };
    static TXvec4 back_v2[] = { { -0.5f,  0.5f, -0.5f, 1.0f },
                                {  0.0f,  0.0f, -1.0f, 0.0f } };
    static TXvec4 back_v3[] = { {  0.5f,  0.5f, -0.5f, 1.0f },
                                {  0.0f,  0.0f, -1.0f, 0.0f } };

    // top face
    static TXvec4 top_v0[] = { { -0.5f, 0.5f,  0.5f, 1.0f },
                               {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 top_v1[] = { {  0.5f, 0.5f,  0.5f, 1.0f },
                               {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 top_v2[] = { {  0.5f, 0.5f, -0.5f, 1.0f },
                               {  0.0f, 1.0f,  0.0f, 0.0f } };
    static TXvec4 top_v3[] = { { -0.5f, 0.5f, -0.5f, 1.0f },
                               {  0.0f, 1.0f,  0.0f, 0.0f } };

    // bottom face
    static TXvec4 bottom_v0[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                  {  0.0f, -1.0f,  0.0f, 0.0f } };
    static TXvec4 bottom_v1[] = { {  0.5f, -0.5f, -0.5f, 1.0f },
                                  {  0.0f, -1.0f,  0.0f, 0.0f } };
    static TXvec4 bottom_v2[] = { {  0.5f, -0.5f,  0.5f, 1.0f },
                                  {  0.0f, -1.0f,  0.0f, 0.0f } };
    static TXvec4 bottom_v3[] = { { -0.5f, -0.5f,  0.5f, 1.0f },
                                  {  0.0f, -1.0f,  0.0f, 0.0f } };

    txDrawQuad(top_v0, top_v1, top_v2, top_v3, TX_POSITION_NORMAL);
    txDrawQuad(front_v0, front_v1, front_v2, front_v3, TX_POSITION_NORMAL);
    txDrawQuad(left_v0, left_v1, left_v2, left_v3, TX_POSITION_NORMAL);
    txDrawQuad(right_v0, right_v1, right_v2, right_v3, TX_POSITION_NORMAL);
    txDrawQuad(back_v0, back_v1, back_v2, back_v3, TX_POSITION_NORMAL);
    txDrawQuad(bottom_v0, bottom_v1, bottom_v2, bottom_v3, TX_POSITION_NORMAL);
}

////////////////////////////////////////
void txDrawColoredCube()
{
    // front face
    static TXvec4 front_v0[] = { { -0.5f, -0.5f, 0.5f, 1.0f },
                                 {  1.0f,  0.0f, 0.0f, 1.0f } };
    static TXvec4 front_v1[] = { {  0.5f, -0.5f, 0.5f, 1.0f },
                                 {  1.0f,  0.0f, 0.0f, 1.0f } };
    static TXvec4 front_v2[] = { {  0.5f,  0.5f, 0.5f, 1.0f },
                                 {  1.0f,  0.0f, 0.0f, 1.0f } };
    static TXvec4 front_v3[] = { { -0.5f,  0.5f, 0.5f, 1.0f },
                                 {  1.0f,  0.0f, 0.0f, 1.0f } };

    // left face
    static TXvec4 left_v0[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                {  0.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 left_v1[] = { { -0.5f, -0.5f,  0.5f, 1.0f },
                                {  0.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 left_v2[] = { { -0.5f,  0.5f,  0.5f, 1.0f },
                                {  0.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 left_v3[] = { { -0.5f,  0.5f, -0.5f, 1.0f },
                                {  0.0f,  1.0f,  0.0f, 1.0f } };

    // right face
    static TXvec4 right_v0[] = { { 0.5f, -0.5f,  0.5f, 1.0f },
                                 { 0.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 right_v1[] = { { 0.5f, -0.5f, -0.5f, 1.0f },
                                 { 0.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 right_v2[] = { { 0.5f,  0.5f, -0.5f, 1.0f },
                                 { 0.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 right_v3[] = { { 0.5f,  0.5f,  0.5f, 1.0f },
                                 { 0.0f,  0.0f,  1.0f, 1.0f } };

    // back face
    static TXvec4 back_v0[] = { {  0.5f, -0.5f, -0.5f, 1.0f },
                                {  1.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 back_v1[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                {  1.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 back_v2[] = { { -0.5f,  0.5f, -0.5f, 1.0f },
                                {  1.0f,  1.0f,  0.0f, 1.0f } };
    static TXvec4 back_v3[] = { {  0.5f,  0.5f, -0.5f, 1.0f },
                                {  1.0f,  1.0f,  0.0f, 1.0f } };

    // top face
    static TXvec4 top_v0[] = { { -0.5f, 0.5f,  0.5f, 1.0f },
                               {  0.0f, 1.0f,  1.0f, 1.0f } };
    static TXvec4 top_v1[] = { {  0.5f, 0.5f,  0.5f, 1.0f },
                               {  0.0f, 1.0f,  1.0f, 1.0f } };
    static TXvec4 top_v2[] = { {  0.5f, 0.5f, -0.5f, 1.0f },
                               {  0.0f, 1.0f,  1.0f, 1.0f } };
    static TXvec4 top_v3[] = { { -0.5f, 0.5f, -0.5f, 1.0f },
                               {  0.0f, 1.0f,  1.0f, 1.0f } };

    // bottom face
    static TXvec4 bottom_v0[] = { { -0.5f, -0.5f, -0.5f, 1.0f },
                                  {  1.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 bottom_v1[] = { {  0.5f, -0.5f, -0.5f, 1.0f },
                                  {  1.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 bottom_v2[] = { {  0.5f, -0.5f,  0.5f, 1.0f },
                                  {  1.0f,  0.0f,  1.0f, 1.0f } };
    static TXvec4 bottom_v3[] = { { -0.5f, -0.5f,  0.5f, 1.0f },
                                  {  1.0f,  0.0f,  1.0f, 1.0f } };

    txDrawQuad(top_v0, top_v1, top_v2, top_v3, TX_POSITION_COLOR);
    txDrawQuad(front_v0, front_v1, front_v2, front_v3, TX_POSITION_COLOR);
    txDrawQuad(left_v0, left_v1, left_v2, left_v3, TX_POSITION_COLOR);
    txDrawQuad(right_v0, right_v1, right_v2, right_v3, TX_POSITION_COLOR);
    txDrawQuad(back_v0, back_v1, back_v2, back_v3, TX_POSITION_COLOR);
    txDrawQuad(bottom_v0, bottom_v1, bottom_v2, bottom_v3, TX_POSITION_COLOR);
}
