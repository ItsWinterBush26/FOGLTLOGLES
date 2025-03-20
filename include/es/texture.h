#pragma once

#include "es/utils.h"

#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

inline struct TextureTargets {
    GLenum target;
    GLenum queryTarget;
} targets[] = {
    { GL_TEXTURE_2D, GL_TEXTURE_2D },
    { GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { GL_TEXTURE_2D_ARRAY, GL_TEXTURE_2D_ARRAY },
    { GL_TEXTURE_3D, GL_TEXTURE_3D }
};

inline void selectProperTexType(GLint internalFormat, GLenum& type) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT24:
            type = GL_UNSIGNED_INT;
            break;
        case 35898: // GL_RGB16F
            // if (ESUtils::version.first != 3) LOGW("GL_RGB16F isn't supported below ES 3");
            type = GL_UNSIGNED_BYTE; // GL_FLOAT;
            break;
    }
}

inline void selectProperTexFormat(GLint internalFormat, GLenum& format) {
    switch (internalFormat) {
        case 5898: // GL_RGB16F
            format = GL_RGB;
            break;
    }
}

inline void selectProperTexIFormat(GLint& internalFormat) {
    switch (internalFormat) {
        case 0x81a7: // GL_DEPTH_COMPONENT32
            internalFormat = GL_DEPTH_COMPONENT24;
            break;
    }
}

inline GLenum getTextureTarget(GLuint texture) {
    if (!texture) return GL_TEXTURE_2D;
    GLint target = 0;

    for (auto& t : targets) {
        glBindTexture(t.target, texture);
        glGetTexLevelParameteriv(t.queryTarget, 0, GL_TEXTURE_WIDTH, &target);
        if (target > 0) return t.target;
    }

    return GL_TEXTURE_2D;
}
