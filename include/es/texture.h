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
    return internalFormat;
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
