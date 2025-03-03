#pragma once

#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

inline struct {
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
            type = GL_UNSIGNED_SHORT;
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
