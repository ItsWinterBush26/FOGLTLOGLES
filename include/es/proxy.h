#pragma once

#include <gl/gl.h>
#include <GLES2/gl2.h>

static inline bool isProxyTexture(GLenum target) {
    switch (target) {
        case GL_PROXY_TEXTURE_1D:
        case GL_PROXY_TEXTURE_2D:
        case GL_PROXY_TEXTURE_3D:
        case GL_PROXY_TEXTURE_RECTANGLE_ARB:
            return true;
        default:
            return false;
    }
}

static GLint inline nlevel(GLint size, GLint level) {
    if (size) {
        size >>= level;
        if (!size) size = 1;
    }

    return size;
}


