// Copyright (C) 2022 saccharineboi

#pragma once

#include <stdbool.h>

////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////

////////////////////////////////////////
enum TXtextureType { TX_TEXTURE_EMISSIVE,
                     TX_TEXTURE_AMBIENT,
                     TX_TEXTURE_DIFFUSE,
                     TX_TEXTURE_SPECULAR };

////////////////////////////////////////
enum TXtextureChannel { TX_TEXTURE_RGB,
                        TX_TEXTURE_RGBA };

////////////////////////////////////////
enum TXtextureWrapMode { TX_TEXTURE_REPEAT,
                         TX_TEXTURE_CLAMP,
                         TX_TEXTURE_MIRROR };

////////////////////////////////////////
enum TXtextureFilterMode { TX_TEXTURE_NEAREST,
                           TX_TEXTURE_LINEAR,
                           TX_TEXTURE_NEAREST_MIPMAP_NEAREST,
                           TX_TEXTURE_NEAREST_MIPMAP_LINEAR,
                           TX_TEXTURE_LINEAR_MIPMAP_NEAREST,
                           TX_TEXTURE_LINEAR_MIPMAP_LINEAR };

////////////////////////////////////////
struct TXtexture_t
{
    unsigned id;
    unsigned char* data;
    unsigned width, height;
    unsigned numBytes;
    enum TXtextureType type;
    enum TXtextureChannel channel;
    enum TXtextureWrapMode wrapMode;
    enum TXtextureFilterMode filterMode;
    int numChannels;
    int numMipmaps;
    unsigned char** mipmaps;
};
typedef struct TXtexture_t TXtexture_t;

////////////////////////////////////////
void txInitTextureEmpty(TXtexture_t* texture);

////////////////////////////////////////
bool txInitTextureInPlace(TXtexture_t* texture,
                          unsigned char* data,
                          unsigned width, unsigned height,
                          unsigned numBytes,
                          int numChannels,
                          enum TXtextureType type,
                          enum TXtextureChannel channel,
                          enum TXtextureWrapMode wrapMode,
                          enum TXtextureFilterMode filterMode);

////////////////////////////////////////
bool txInitTextureSTB(TXtexture_t* texture,
                      const char* url,
                      enum TXtextureType type,
                      enum TXtextureWrapMode wrapMode,
                      enum TXtextureFilterMode filterMode);

////////////////////////////////////////
void txFreeTexture(TXtexture_t* texture);

////////////////////////////////////////
#ifdef __cplusplus
}
#endif
////////////////////////////////////////
