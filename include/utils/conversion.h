#pragma once

#include "extensions.h"
#include "gltypes.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

struct TextureConversionResult {
    GLenum internalFormat; // ES internal format to use (unsized, e.g. GL_RGBA, GL_RGB, etc.)
    GLenum format;         // ES pixel format
    GLenum type;           // ES pixel type (e.g. GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, etc.)
    void* data;            // Converted pixel data buffer
};

// Convert texture data from a desktop “sized” format to ES–compatible parameters.
// This function maps the source internal format, format, and type to target ES values,
// and if necessary, converts the pixel data.
static inline TextureConversionResult convertTextureData(GLenum srcInternalFormat,
                                             GLenum srcFormat,
                                             GLenum srcType,
                                             const void* srcData,
                                             int width,
                                             int height) {
    TextureConversionResult result;
    int numPixels = width * height;
    int targetChannels = 0;
    GLenum targetType = GL_UNSIGNED_BYTE; // Default for color textures

    switch(srcInternalFormat) {
        // Group 1: RGBA formats
        case GL_RGBA8:
        case GL_RGBA4:
            result.internalFormat = GL_RGBA;
            result.format = GL_RGBA;
            targetChannels = 4;
            break;
        case GL_RGBA32F:
            if (ESExtensions::isSupported("GL_OES_texture_float")) {
                result.internalFormat = GL_RGBA32F;
                result.format = GL_RGBA;
                targetChannels = 4;
                targetType = GL_FLOAT;
            } else {
                result.internalFormat = GL_RGBA;
                result.format = GL_RGBA;
                targetChannels = 4;
            }
            break;
        case GL_RGBA16F:
            if (ESExtensions::isSupported("GL_OES_texture_half_float")) {
                result.internalFormat = GL_RGBA16F;
                result.format = GL_RGBA;
                targetChannels = 4;
                // GL_HALF_FLOAT_OES is usually defined in <GLES2/gl2ext.h>
                targetType = GL_HALF_FLOAT_OES;
            } else {
                result.internalFormat = GL_RGBA;
                result.format = GL_RGBA;
                targetChannels = 4;
            }
            break;
        case GL_SRGB8_ALPHA8:
            if (ESExtensions::isSupported("GL_EXT_sRGB")) {
                result.internalFormat = GL_SRGB8_ALPHA8;
                result.format = GL_RGBA;
                targetChannels = 4;
            } else {
                result.internalFormat = GL_RGBA;
                result.format = GL_RGBA;
                targetChannels = 4;
            }
            break;
        // Group 2: RGB formats
        case GL_RGB8:
        case GL_RGB5_A1:
        case GL_RGB565:
            result.internalFormat = GL_RGB;
            result.format = GL_RGB;
            targetChannels = 3;
            break;
        case GL_SRGB8:
            if (ESExtensions::isSupported("GL_EXT_sRGB")) {
                result.internalFormat = GL_SRGB8;
                result.format = GL_RGB;
                targetChannels = 3;
            } else {
                result.internalFormat = GL_RGB;
                result.format = GL_RGB;
                targetChannels = 3;
            }
            break;
        // Group 3: Depth formats
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
            result.internalFormat = GL_DEPTH_COMPONENT;
            result.format = GL_DEPTH_COMPONENT;
            targetChannels = 1;
            targetType = GL_UNSIGNED_SHORT; // Use 16-bit depth
            break;
        // Group 4: Single-channel red formats
        case GL_R8:
        case GL_R16:
            if (ESExtensions::isSupported("GL_EXT_texture_rg"))
                result.internalFormat = result.format = GL_RED_EXT;
            else
                result.internalFormat = result.format = GL_LUMINANCE;
            targetChannels = 1;
            break;
        // Group 5: Two-channel red-green formats
        case GL_RG8:
        case GL_RG16:
            if (ESExtensions::isSupported("GL_EXT_texture_rg"))
                result.internalFormat = GL_RG8; // Assuming unsized GL_RG is available
            else
                result.internalFormat = result.format = GL_LUMINANCE_ALPHA;
            targetChannels = 2;
            break;
        default:
            result.internalFormat = GL_RGBA;
            result.format = GL_RGBA;
            targetChannels = 4;
            break;
    }
    result.type = targetType;

    // Determine the size for each channel.
    int targetChannelSize = (targetType == GL_UNSIGNED_BYTE) ? sizeof(unsigned char) :
                             (targetType == GL_UNSIGNED_SHORT) ? sizeof(unsigned short) :
                             (targetType == GL_FLOAT) ? sizeof(float) : 0;
    size_t targetSize = numPixels * targetChannels * targetChannelSize;

    // If the source type already matches the target type, we can do a direct copy.
    bool directCopy = (srcType == targetType);
    result.data = malloc(targetSize);
    if(directCopy) {
        memcpy(result.data, srcData, targetSize);
    } else {
        // Handle some common conversion cases.
        if(srcType == GL_FLOAT && targetType == GL_UNSIGNED_BYTE) {
            const float* src = (const float*) srcData;
            unsigned char* dst = (unsigned char*) result.data;
            for (int i = 0; i < numPixels * targetChannels; i++) {
                float val = src[i];
                if(val < 0.0f) val = 0.0f;
                if(val > 1.0f) val = 1.0f;
                dst[i] = (unsigned char)(val * 255.0f + 0.5f);
            }
        } else if(srcType == GL_UNSIGNED_SHORT && targetType == GL_UNSIGNED_BYTE) {
            const unsigned short* src = (const unsigned short*) srcData;
            unsigned char* dst = (unsigned char*) result.data;
            for (int i = 0; i < numPixels * targetChannels; i++) {
                dst[i] = (unsigned char)(src[i] / 257);
            }
        } else if(srcType == GL_UNSIGNED_INT && targetType == GL_UNSIGNED_BYTE) {
            const unsigned int* src = (const unsigned int*) srcData;
            unsigned char* dst = (unsigned char*) result.data;
            for (int i = 0; i < numPixels * targetChannels; i++) {
                dst[i] = (unsigned char)(src[i] / 16777216u);
            }
        } else {
            memcpy(result.data, srcData, targetSize);
        }
    }
    return result;
}
