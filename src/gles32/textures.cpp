#include "es/framebuffer.h"
#include "es/proxy.h"
#include "es/state_tracking.h"
#include "gles20/framebuffer_tracking.h"
#include "gles30/read_pixels.h"
#include "gles32/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>

void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params);

// this should be in gles3.1 but idrc
void GLES32::registerTextureOverrides() {
    REGISTER(glGetTexImage);

    REGISTEROV(glGetTexLevelParameteriv);
    REGISTEROV(glGetTexLevelParameterfv);
}

void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels) {
    if (format != GL_RGBA
     && format != GL_RGBA_INTEGER
     && type != GL_UNSIGNED_BYTE
     && type != GL_UNSIGNED_INT
     && type != GL_INT
     && type != GL_FLOAT) return;

    OV_glBindFramebuffer(GL_READ_FRAMEBUFFER, fakeDepthbuffer->readFramebuffer);
    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        target,
        trackedStates->activeTextureState.boundTextures[target],
        level
    );

    GLsizei width, height;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);

    glReadPixels(0, 0, width, height, format, type, pixels);
    
    glFramebufferRenderbuffer(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        0
    );
}

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) {
    LOGI("glGetTexLevelParameteriv: target=%u level=%i pname=%u", target, level, pname);
    
    if (isProxyTexture(target)) {
        getProxyTexLevelParameter(pname, params);
        return;
    }

    glGetTexLevelParameteriv(target, level, pname, params);
}

void OV_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params) {
    LOGI("glGetTexLevelParameterfv: target=%u level=%i pname=%u", target, level, pname);
    
    if (isProxyTexture(target)) {
        GLint tmp = 0;
        getProxyTexLevelParameter(pname, &tmp);

        (*params) = static_cast<GLfloat>(tmp);
        return;
    }

    glGetTexLevelParameterfv(target, level, pname, params);
}
