#include "gles20/main.h"
#include "es/proxy.h"
#include "es/texture.h"
#include "es/texparam.h"
#include "main.h"
#include "utils/log.h"
#include "utils/pointers.h"

#include <GLES2/gl2.h>
#include <cstring>
#include <memory>
#include <vector>

void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void OV_glTexParameteri(GLenum target, GLenum pname, GLint param);

void OV_glDeleteTextures(GLsizei n, const GLuint *textures);

inline GLint maxTextureSize = 0;

void GLES20::registerTextureOverrides() {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOGI("GL_MAX_TEXTURE_SIZE is %i", maxTextureSize);

    REGISTEROV(glTexImage2D);
    
    REGISTEROV(glTexParameterf);
    // REGISTEROV(glTexParameteri);

    REGISTEROV(glDeleteTextures);
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
        boundProxyTexture = MakeAggregateShared<ProxyTexture>(
            target,
            (( width << level ) > maxTextureSize) ? 0 : width,
            (( height << level ) > maxTextureSize) ? 0 : height,
            level,
            internalFormat
        );
    } else {
        fixTexArguments(target, internalFormat, type, format);
        glTexImage2D(
            target, level, internalFormat, 
            width, height,
            border, format, type, pixels
        );
    }

    trackTextureFormat(internalFormat);
}

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    LOGI("glTexParameterf: target=%u pname=%u param=%f", target, pname, param);
    
    selectProperTexParamf(target, pname, param);
    glTexParameterf(target, pname, param);
}

void OV_glTexParameteri(GLenum target, GLenum pname, GLint param) {
    // LOGI("glTexParameteri: target=%u pname=%u param=%d", target, pname, param);

    selectProperTexParami(target, pname, param);
    glTexParameteri(target, pname, param);
}

void OV_glDeleteTextures(GLsizei n, const GLuint *textures) {
    glDeleteTextures(n, textures);

    for (GLint i = 0; i < n; ++i) {
        trackedTextures.erase(textures[i]);
    }
}
