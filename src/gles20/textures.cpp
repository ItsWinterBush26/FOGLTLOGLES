#include "gles20/main.h"
#include "es/proxy.h"
#include "es/texture.h"
#include "es/texparam.h"
#include "main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <memory>

void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void OV_glTexParameteri(GLenum target, GLenum pname, GLint param);

inline GLint maxTextureSize = 0;

void GLES20::registerTextureOverrides() {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOGI("GL_MAX_TEXTURE_SIZE is %i", maxTextureSize);

    REGISTEROV(glTexImage2D);
    REGISTEROV(glTexImage3D);
    
    REGISTEROV(glTexParameterf);
    REGISTEROV(glTexParameteri);
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
        boundProxyTexture = std::make_shared<ProxyTexture>(
            target,
            (( width << level ) > maxTextureSize) ? 0 : width,
            (( height << level ) > maxTextureSize) ? 0 : height,
            0,
            level,
            internalFormat
        );
    } else {
        selectProperTexIFormat(internalFormat);
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
        boundProxyTexture = std::make_shared<ProxyTexture>(
            target,
            (( width << level ) > maxTextureSize) ? 0 : width,
            (( height << level ) > maxTextureSize) ? 0 : height,
            depth,
            level,
            internalFormat
        );
    } else {
        selectProperTexIFormat(internalFormat);
        selectProperTexFormat(internalFormat, format);
        selectProperTexType(internalFormat, type);
        glTexImage3D(
            target, level, internalFormat,
            width, height, depth, 
            border, format, type, pixels
        );
    }
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
