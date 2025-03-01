#pragma once

#include <GLES2/gl2.h>
#include "gl/header.h"

inline void selectProperTexType(GLint internalFormat, GLenum& type) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            type = GL_UNSIGNED_SHORT;
            break;
        case GL_DEPTH_COMPONENT16:
            type = GL_UNSIGNED_SHORT;
            break;
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            type = GL_UNSIGNED_INT;
            break;
        case GL_RGBA16F:
        case GL_RGB16F:
            type = GL_HALF_FLOAT;
            break;
        case GL_RGBA32F:
        case GL_RGB32F:
            type = GL_FLOAT;
            break;
        case GL_R8:
        case GL_RG8:
        case GL_RGB8:
        case GL_RGBA8:
            type = GL_UNSIGNED_BYTE;
            break;
    }
}

inline GLenum getProperTexFormat(GLint internalFormat) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT;
        case GL_R8:
        case GL_R16F:
        case GL_R32F:
            return GL_RED;
        case GL_RG8:
        case GL_RG16F:
        case GL_RG32F:
            return GL_RG;
        case GL_RGB8:
        case GL_RGB16F:
        case GL_RGB32F:
            return GL_RGB;
        case GL_RGBA8:
        case GL_RGBA16F:
        case GL_RGBA32F:
            return GL_RGBA;
        default:
            return internalFormat; // Default to the same format
    }
}
