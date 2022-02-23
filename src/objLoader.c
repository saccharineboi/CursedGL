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

#include "objLoader.h"
#include "vec.h"
#include "rasterizer.h"

#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////
enum vertex_type { v_position, v_normal, v_texCoord, v_face };

////////////////////////////////////////
static int countNumVertices(FILE* objFile, enum vertex_type type)
{
    char* line = NULL;
    size_t nRead;
    int numVertices = 0;

    switch (type) {
        case v_position:
            while (getline(&line, &nRead, objFile) > 0)
                if (line[0] == 'v' && line[1] == ' ')
                    ++numVertices;
            break;
        case v_normal:
            while (getline(&line, &nRead, objFile) > 1 )
                if (line[0] == 'v' && line[1] == 't')
                    ++numVertices;
            break;
        case v_texCoord:
            while (getline(&line, &nRead, objFile) > 1)
                if (line[0] == 'v' && line[1] == 'n')
                    ++numVertices;
            break;
        case v_face:
            while (getline(&line, &nRead, objFile) > 0)
                if (line[0] == 'f')
                    ++numVertices;
            break;
    }

    free(line);
    rewind(objFile);
    return numVertices;
}

////////////////////////////////////////
static void loadPositions(FILE* objFile, int numPositions, float* positions)
{
    char* line = NULL;
    size_t len;
    int ctr = 0;
    for (int i = 0; i < numPositions && getline(&line, &len, objFile) > 0;) {
        if (line[0] == 'v') {
            sscanf(line, "v %f %f %f", &positions[ctr],
                                       &positions[ctr + 1],
                                       &positions[ctr + 2]);
            ctr += 3;
            ++i;
        }
    }
    free(line);
    rewind(objFile);
}

////////////////////////////////////////
static void loadNormals(FILE* objFile, int numNormals, float* normals)
{
    char* line = NULL;
    size_t len;
    int ctr = 0;
    for (int i = 0; i < numNormals && getline(&line, &len, objFile) > 1;) {
        if (line[0] == 'v' && line[1] == 'n') {
            sscanf(line, "vn %f %f %f", &normals[ctr],
                                        &normals[ctr + 1],
                                        &normals[ctr + 2]);
            ctr += 3;
            ++i;
        }
    }
    free(line);
    rewind(objFile);
}

////////////////////////////////////////
static void loadTexCoords(FILE* objFile, int numTexCoords, float* texCoords)
{
    char* line = NULL;
    size_t len;
    int ctr = 0;
    for (int i = 0; i < numTexCoords && getline(&line, &len, objFile) > 1;) {
        if (line[0] == 'v' && line[1] == 't') {
            sscanf(line, "vt %f %f", &texCoords[ctr],
                                     &texCoords[ctr + 1]);
            ctr += 2;
            ++i;
        }
    }
    free(line);
    rewind(objFile);
}

////////////////////////////////////////
static void loadFaces(FILE* objFile, int numFaces, unsigned* indices)
{
    char* line = NULL;
    size_t len;
    int ctr = 0;
    for (int i = 0; i < numFaces && getline(&line, &len, objFile) > 0;) {
        if (line[0] == 'f') {
            sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u", &indices[ctr],
                                                         &indices[ctr + 1],
                                                         &indices[ctr + 2],
                                                         &indices[ctr + 3],
                                                         &indices[ctr + 4],
                                                         &indices[ctr + 5],
                                                         &indices[ctr + 6],
                                                         &indices[ctr + 7],
                                                         &indices[ctr + 8]);
            ctr += 9;
            ++i;
        }
    }
    free(line);
    rewind(objFile);
}

////////////////////////////////////////
bool txLoadObjModel(const char* filePath, TXobjModel_t* model)
{
    FILE* objFile = fopen(filePath, "r");
    if (!objFile) return false;

    int numPositions    = countNumVertices(objFile, v_position);
    int numNormals      = countNumVertices(objFile, v_normal);
    int numTexCoords    = countNumVertices(objFile, v_texCoord);
    int numFaces        = countNumVertices(objFile, v_face);

    model->positions    = malloc((size_t)(3 * numPositions * (int)sizeof(float)));
    model->normals      = malloc((size_t)(3 * numNormals   * (int)sizeof(float)));
    model->texCoords    = malloc((size_t)(2 * numTexCoords * (int)sizeof(float)));
    model->indices      = malloc((size_t)(9 * numFaces     * (int)sizeof(unsigned)));

    if (!model->positions   ||
        !model->normals     ||
        !model->texCoords   ||
        !model->indices)
    {
        txFreeObjModel(model);
        return false;
    }

    loadPositions(objFile, numPositions, model->positions);
    loadNormals(objFile, numNormals, model->normals);
    loadTexCoords(objFile, numTexCoords, model->texCoords);
    loadFaces(objFile, numFaces, model->indices);
    model->numIndices = (unsigned)numFaces;

    fclose(objFile);
    return true;
}

////////////////////////////////////////
void txFreeObjModel(TXobjModel_t* model)
{
    if (model) {
        free(model->positions);
        free(model->normals);
        free(model->texCoords);
        free(model->indices);

        model->numIndices = 0;
    }
}

////////////////////////////////////////
void txRenderObjModel(TXobjModel_t* model)
{
    unsigned indexCount = model->numIndices * 9;
    for (unsigned i = 0; i < indexCount; i += 9) {
        unsigned firstVertexId  = model->indices[i];
        unsigned secondVertexId = model->indices[i + 3];
        unsigned thirdVertexId  = model->indices[i + 6];

        TXvec4 v0[] = {
            {
                model->positions[(firstVertexId - 1) * 3 + 0],
                model->positions[(firstVertexId - 1) * 3 + 1],
                model->positions[(firstVertexId - 1) * 3 + 2],
                1.0f
            },
            {
                model->normals[(firstVertexId - 1) * 3 + 0],
                model->normals[(firstVertexId - 1) * 3 + 1],
                model->normals[(firstVertexId - 1) * 3 + 2]
            }
        };

        TXvec4 v1[] = {
            {
                model->positions[(secondVertexId - 1) * 3 + 0],
                model->positions[(secondVertexId - 1) * 3 + 1],
                model->positions[(secondVertexId - 1) * 3 + 2],
                1.0f
            },
            {
                model->normals[(firstVertexId - 1) * 3 + 0],
                model->normals[(firstVertexId - 1) * 3 + 1],
                model->normals[(firstVertexId - 1) * 3 + 2]
            }
        };

        TXvec4 v2[] = {
            {
                model->positions[(thirdVertexId - 1) * 3 + 0],
                model->positions[(thirdVertexId - 1) * 3 + 1],
                model->positions[(thirdVertexId - 1) * 3 + 2],
                1.0f
            },
            {
                model->normals[(firstVertexId - 1) * 3 + 0],
                model->normals[(firstVertexId - 1) * 3 + 1],
                model->normals[(firstVertexId - 1) * 3 + 2]
            }
        };

        txDrawTriangle(v0, v1, v2, TX_POSITION_NORMAL);
    }
}
