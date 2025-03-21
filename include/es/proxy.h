#pragma once

#include <gl/gl.h>

#include <GLES2/gl2.h>
#include <memory>

struct ProxyTexture {
    GLenum type;
    GLint width;
    GLint height;
    // GLint depth; // if GL_PROXY_TEXTURE_3D
    GLint level;
    GLint internalFormat;
};

inline std::shared_ptr<ProxyTexture> boundProxyTexture = nullptr; // std::make_shared<ProxyTexture>();

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
    if (size) {
        size >>= level;
        if (!size) size = 1;
    }
    return size;
}

inline void getProxyTexLevelParameter(GLenum pname, GLint* params) {
    switch (pname) {
        case GL_TEXTURE_WIDTH:
            (*params) = nlevel(boundProxyTexture->width, boundProxyTexture->level);
            return;
        case GL_TEXTURE_HEIGHT:
            (*params) = nlevel(boundProxyTexture->height, boundProxyTexture->level);
            return;
        /* case GL_TEXTURE_DEPTH:
            if (boundProxyTexture->type != GL_TEXTURE_3D) return;
            (*params) = nlevel(boundProxyTexture->depth, boundProxyTexture->level);
            return; */
        case GL_TEXTURE_INTERNAL_FORMAT:
            (*params) = boundProxyTexture->internalFormat;
            return;
    }
}
