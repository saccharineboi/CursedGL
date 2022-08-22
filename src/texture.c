// Copyright (C) 2022 saccharineboi

#include "texture.h"
#include "error.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>

////////////////////////////////////////
static unsigned currentTextureId = 1;

////////////////////////////////////////
static bool processTexture(TXtexture_t* texture)
{
    if (texture->filterMode != TX_TEXTURE_NEAREST && texture->filterMode != TX_TEXTURE_LINEAR) {
        texture->numMipmaps = (int)log2(texture->width);
        if (texture->numMipmaps <= 0) {
            txOutputMessage(TX_WARNING, "[CursedGL] processTexture: invalid number of mipmaps: %d", texture->numMipmaps);
        }
        texture->mipmaps = (unsigned char**)malloc((unsigned)texture->numMipmaps * sizeof(unsigned char*));
        if (!texture->mipmaps) {
            txOutputMessage(TX_ERROR, "[CursedGL] processTexture: not enough memory for mipmaps array");
            return false;
        }

        unsigned mipmapWidth = texture->width / 2, mipmapHeight = texture->height / 2;
        for (int i = 0; i < texture->numMipmaps; ++i) {
            texture->mipmaps[i] = (unsigned char*)malloc(mipmapWidth * mipmapHeight * (unsigned)texture->numChannels);
            if (!texture->mipmaps[i]) {
                txOutputMessage(TX_ERROR, "[CursedGL] processTexture: not enough memory for mipmap %i for texture %u", i, texture->id);
                return false;
            }
            txOutputMessage(TX_INFO, "[CursedGL] processTexture: generated mipmap (%u:%u) for texture %u", mipmapWidth, mipmapHeight, texture->id);
            mipmapWidth /= 2;
            mipmapHeight /= 2;
        }
        txOutputMessage(TX_INFO, "[CursedGL] processTexture: generated %d mipmaps for texture %u", texture->numMipmaps, texture->id);
    }
    else {
        texture->numMipmaps = 0;
        texture->mipmaps = NULL;
    }
    txOutputMessage(TX_INFO, "[CursedGL] processTexture: generated texture %u (%u bytes)", texture->id, texture->numBytes);
    return true;
}

////////////////////////////////////////
bool txInitTextureInPlace(TXtexture_t* texture,
                          unsigned char* data,
                          unsigned width, unsigned height,
                          unsigned numBytes,
                          int numChannels,
                          enum TXtextureType type,
                          enum TXtextureChannel channel,
                          enum TXtextureWrapMode wrapMode,
                          enum TXtextureFilterMode filterMode)
{
    if (!texture) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureInPlace: texture handle is NULL");
        return false;
    }
    else if (width != height || (width & (width - 1))) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureInPlace: size of texture must be power of 2");
        return false;
    }
    else if (!data) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureInPlace: texture data is NULL");
        return false;
    }

    texture->id = currentTextureId++;
    texture->numBytes = numBytes;

    texture->data = (unsigned char*)malloc(numBytes);
    if (!texture->data) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureInPlace: not enough memory for %u bytes for texture %u", numBytes, texture->id);
        return false;
    }
    memcpy(texture->data, data, numBytes);

    texture->width = width;
    texture->height = height;
    texture->type = type;
    texture->channel = channel;
    texture->wrapMode = wrapMode;
    texture->filterMode = filterMode;
    texture->numChannels = numChannels;
    return processTexture(texture);
}

////////////////////////////////////////
bool txInitTextureSTB(TXtexture_t* texture,
                      const char* url,
                      enum TXtextureType type,
                      enum TXtextureWrapMode wrapMode,
                      enum TXtextureFilterMode filterMode)
{
    texture->id = currentTextureId++;

    int numChannels;
    int twidth, theight;
    unsigned char* textureData = stbi_load(url, &twidth, &theight, &numChannels, 0);
    if (!textureData) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureSTB: stb_image.h couldn't load the texture at %s", url);
        return false;
    }
    texture->width = (unsigned)twidth;
    texture->height = (unsigned)theight;

    texture->data = (unsigned char*)malloc((size_t)(twidth * theight * numChannels));
    if (!texture->data) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInitTextureSTB: not enough memory to store texture %u at %s", texture->id, url);
        return false;
    }
    memcpy(texture->data, textureData, (size_t)(twidth * theight * numChannels));
    stbi_image_free(textureData);

    txOutputMessage(TX_INFO, "[CursedGL] txInitTextureSTB: successfully loaded texture %u at %s (%u bytes)", texture->id, url, twidth * theight * numChannels);

    texture->numBytes = texture->width * texture->height * (unsigned)numChannels;
    texture->type = type;
    texture->wrapMode = wrapMode;
    texture->filterMode = filterMode;
    texture->numChannels = numChannels;

    switch (texture->numChannels) {
        case 3:
            texture->channel = TX_TEXTURE_RGB;
            txOutputMessage(TX_INFO, "[CursedGL] txInitTextureSTB: texture %u is set to RGB", texture->id);
            break;
        case 4:
            texture->channel = TX_TEXTURE_RGBA;
            txOutputMessage(TX_INFO, "[CursedGL] txInitTextureSTB: texture %u is set to RGBA", texture->id);
            break;
        default:
            texture->channel = TX_TEXTURE_RGB;
            txOutputMessage(TX_WARNING, "[CursedGL] txInitTextureSTB: texture %u is of unknown channel, setting to RGB anyways", texture->id);
            break;
    }

    return processTexture(texture);
}

////////////////////////////////////////
void txFreeTexture(TXtexture_t* texture)
{
    if (texture->data)
        free(texture->data);
    for (int i = 0; i < texture->numMipmaps; ++i) {
        txOutputMessage(TX_INFO, "[CursedGL] txFreeTexture: removed mipmap %i from texture %u", i, texture->id);
        free(texture->mipmaps[i]);
    }
    if (texture->mipmaps)
        free(texture->mipmaps);
    txOutputMessage(TX_INFO, "[CursedGL] txFreeTexture: removed texture %u (%u bytes)", texture->id, texture->numBytes);
}

////////////////////////////////////////
void txInitTextureEmpty(TXtexture_t* texture)
{
    bzero(texture, sizeof(TXtexture_t));
}
