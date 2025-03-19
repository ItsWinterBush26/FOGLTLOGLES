#include "gles20/main.h"
#include "es/proxy.h"
#include "es/texture.h"
#include "es/texparam.h"
#include "main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>

void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void OV_glTexParameteri(GLenum target, GLenum pname, GLint param);
void glBindTextures(GLuint first, GLsizei count, const GLuint* textures);

static GLint maxTextureSize = 0;
GLint proxyWidth, proxyHeight, proxyDepth, proxyInternalFormat;

void GLES20::registerTextureOverrides() {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOGI("GL_MAX_TEXTURE_SIZE is %i", maxTextureSize);

    REGISTER(glBindTextures);

    REGISTEROV(glTexImage2D);
    REGISTEROV(glTexImage3D);
    REGISTEROV(glGetTexLevelParameteriv);
    REGISTEROV(glTexParameterf);
    // REGISTEROV(glTexParameteri);
}

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    LOGI("glTexParameterf: target=%u pname=%u param=%f", target, pname, param);
    
    selectProperTexParamf(target, pname, param);
    glTexParameterf(target, pname, param);
}

void OV_glTexParameteri(GLenum target, GLenum pname, GLint param) {
    LOGI("glTexParameteri: target=%u pname=%u param=%d", target, pname, param);

    selectProperTexParami(target, pname, param);
    glTexParameteri(target, pname, param);
}

void OV_glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width, GLsizei height,
    GLint border, GLenum format,
    GLenum type, const void* pixels
) {
    LOGI("glTexImage2D: internalformat=%i border=%i format=%i type=%u", internalFormat, border, format, type);
    if (isProxyTexture(target)) {
        LOGI("Received proxy texture of 2D");
        proxyWidth = (( width << level ) > maxTextureSize) ? 0 : width;
        proxyHeight = (( height << level ) > maxTextureSize) ? 0 : height;
        proxyInternalFormat = internalFormat;
    } else {
        selectProperTexFormat(internalFormat, format);
        selectProperTexType(internalFormat, type);
        glTexImage2D(
            target, level, internalFormat, 
            width, height,
            border, format, type, pixels
        );
    }
}

void OV_glTexImage3D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLenum format,
    GLenum type, const void* pixels
) {
    LOGI("glTexImage3D: internalformat=%i border=%i format=%i type=%u", internalFormat, border, format, type);
    
    if (isProxyTexture(target)) {
        LOGI("Received proxy texture of 3D");
        proxyWidth = ((width << level) > maxTextureSize) ? 0 : width;
        proxyHeight = ((height << level) > maxTextureSize) ? 0 : height;
        proxyDepth = depth;
        proxyInternalFormat = internalFormat;
    } else {
        selectProperTexType(internalFormat, type);
        glTexImage3D(
            target, level, internalFormat,
            width, height, depth, 
            border, format, type, pixels
        );
    }
}

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) {
    LOGI("glGetTexLevelParameteriv: target=%u level=%i pname=%u", target, level, pname);
    
    if (isProxyTexture(target)) {
        switch (pname) {
            case GL_TEXTURE_WIDTH:
                (*params) = nlevel(proxyWidth, level);
                return;
            case GL_TEXTURE_HEIGHT:
                (*params) = nlevel(proxyHeight, level);
                return;
            case GL_TEXTURE_DEPTH:
                (*params) = nlevel(proxyDepth, level);
                return;
            case GL_TEXTURE_INTERNAL_FORMAT:
                (*params) = proxyInternalFormat;
                return;
        }
    }

    glGetTexLevelParameteriv(target, level, pname, params);
}

void glBindTextures(GLuint first, GLsizei count, const GLuint* textures) {
    LOGI("glBindTextures: first=%u, count=%i, textures=%p", first, count, textures);

    GLint prevActiveTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveTexture);

    for (GLsizei i = 0; i < count; ++i) {
        GLuint texture = textures ? textures[i] : 0;
        glActiveTexture(GL_TEXTURE0 + first + i);

        if (texture != 0) {
            GLenum target = getTextureTarget(texture);
            glBindTexture(target, texture);
        } else {
            // Unbind from all possible texture targets
            for (GLenum target : {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D }) {
                glBindTexture(target, 0);
            }
        }
    }

    glActiveTexture(prevActiveTexture);
}
