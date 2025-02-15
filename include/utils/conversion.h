#pragma once
#include "extensions.h"

#include <GLES2/gl2ext.h>

#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif

#ifndef GL_RGB8
#define GL_RGB8 0x8051
#endif

#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24 0x81A6
#endif

#ifndef GL_DEPTH_COMPONENT32
#define GL_DEPTH_COMPONENT32 0x81A7
#endif

#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif

#ifndef GL_R8
#define GL_R8 0x8229
#endif

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8D9F
#endif

#ifndef GL_SRGB8
#define GL_SRGB8 0x8C41
#endif

#ifndef GL_RED_EXT
#define GL_RED_EXT 0x1903
#endif

inline GLenum get_es_internalformat(GLenum gl_iform) {
	switch (gl_iform) {
        case GL_RGBA8:
        case GL_RGBA:
            return GL_RGBA;
        case GL_RGB8:
        case GL_RGB:
            return GL_RGB;
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
            return GL_DEPTH_COMPONENT;
        case GL_R8:
            return ESExtensions::isSupported("GL_EXT_texture_rg") ? GL_RED_EXT : GL_LUMINANCE;
        case GL_RGBA32F:
            return ESExtensions::isSupported("GL_OES_texture_float") ? GL_RGBA32F : GL_RGBA;
        case GL_RGBA16F:
            return ESExtensions::isSupported("GL_OES_texture_half_float") ? GL_RGBA16F : GL_RGBA;
        case GL_SRGB8_ALPHA8:
            return ESExtensions::isSupported("GL_EXT_sRGB") ? GL_SRGB_ALPHA_EXT : GL_RGBA;
        case GL_SRGB8:
            return ESExtensions::isSupported("GL_EXT_sRGB") ? GL_SRGB8 : GL_RGB;
        default:
            return GL_RGBA;
    }
};
