#pragma once

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include "es/utils.h"
#include "gl/header.h"
#include "gl/glext.h"

inline void selectProperTexType(GLint internalFormat, GLenum& type) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
            type = GL_UNSIGNED_SHORT;
            break;

        case GL_DEPTH_COMPONENT24:
            // GLES 3.2 does not support GL_DEPTH_COMPONENT24 without an extension.
            if (ESUtils::isExtensionSupported("GL_OES_depth24")) {
                type = GL_UNSIGNED_INT;
            } else {
                type = GL_UNSIGNED_SHORT; // Fallback
            }
            break;

        case GL_DEPTH_COMPONENT32F:
            // GLES 3.2 only supports GL_DEPTH_COMPONENT32F if GL_OES_depth32 is available.
            if (ESUtils::isExtensionSupported("GL_OES_depth32")) {
                type = GL_FLOAT;
            } else {
                type = GL_UNSIGNED_INT; // Fallback
            }
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

inline GLenum selectProperTexFormat(GLint internalFormat) {
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
            return GL_RGBA;
    }
}
