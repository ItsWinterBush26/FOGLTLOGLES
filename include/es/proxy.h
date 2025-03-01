#pragma once

#include <gl/gl.h>
#include <GLES2/gl2.h>

inline bool isProxyTexture(GLenum target) {
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

inline GLint nlevel(GLint size, GLint level) {
    if (size == 0) return 0;
    
    GLint result = size >> level;
    return result > 0 ? result : 1;
}
