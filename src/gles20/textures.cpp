#include "es/ffpe/lists.hpp"
#include "es/ffpe/states.hpp"
#include "es/framebuffer.hpp"
#include "es/proxy.hpp"
#include "es/state_tracking.hpp"
#include "es/swizzling.hpp"
#include "es/texture.hpp"
#include "es/texparam.hpp"
#include "gles20/main.hpp"
#include "main.hpp"
#include "utils/log.hpp"
#include "utils/pointers.hpp"

#include <GLES2/gl2.h>

void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
void OV_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void OV_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void OV_glTexParameteri(GLenum target, GLenum pname, GLint param);

void GLES20::registerTextureOverrides() {
    REGISTEROV(glTexImage2D);
    REGISTEROV(glTexSubImage2D);
    REGISTEROV(glCopyTexImage2D);
    REGISTEROV(glCopyTexSubImage2D);
    
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
    if (isProxyTexture(target)) {
        boundProxyTexture = MakeAggregateShared<ProxyTexture>(
            target,
            (( width << level ) > ESLimits::MAX_TEXTURE_SIZE) ? 0 : width,
            (( height << level ) > ESLimits::MAX_TEXTURE_SIZE) ? 0 : height,
            level,
            internalFormat
        );
    } else {
        // LOGI("glTexImage2D (before): internalformat=%i border=%i format=%i type=%u", internalFormat, border, format, type);
        
        fixTexArguments(target, internalFormat, type, format, (width == height));
        glTexImage2D(
            target, level, internalFormat, 
            width, height,
            border, format, type, pixels
        );

        // LOGI("glTexImage2D (after): internalformat=%i border=%i format=%i type=%u", internalFormat, border, format, type);
    }

    trackTextureFormat(internalFormat);
}

void OV_glTexSubImage2D(
    GLenum target, GLint level, 
    GLint xOffset, GLint yOffset, 
    GLsizei width, GLsizei height, 
    GLenum format, GLenum type,
    const void* pixels
) {
    if (format == 0x80e1) { // GL_BGRA
        swizzleBGRA(type);
        doSwizzling(target);

        format = GL_RGBA;
    }

    if (isDepthFormat(format)
        && width == fakeDepthbuffer->width
        && height == fakeDepthbuffer->height
        && fakeDepthbuffer->data == pixels
    ) {
        fakeDepthbuffer->blitFakeReadToFakeDraw(target, level, xOffset, yOffset, width, height);
        return;
    }

    glTexSubImage2D(
        target, level,
        xOffset, yOffset,
        width, height,
        format, type,
        pixels
    );
}

void OV_glCopyTexImage2D(
    GLenum target, GLint level,
    GLenum internalformat,
    GLint x, GLint y,
    GLsizei width, GLsizei height, 
    GLint border
) {
    if (isDepthFormat(internalformat)) {
        OV_glTexImage2D(
            target, level, trackedStates->activeTextureState->getTextureInternalFormat(GL_TEXTURE_2D),
            width, height, border,
            GL_DEPTH_COMPONENT32F, GL_FLOAT,
            nullptr
        );

        fakeDepthbuffer->blitCurrentReadToFakeDraw(target, level, x, y, width, height);
        return;
    }
    
    glCopyTexImage2D(
        target,  level, internalformat,
        x, y,
        width, height, border
    );
}

void OV_glCopyTexSubImage2D(
    GLenum target, GLint level,
    GLint xoffset, GLint yoffset,
    GLint x, GLint y,
    GLsizei width, GLsizei height
) {
    glGetError();
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    if (glGetError() == GL_INVALID_OPERATION) {
        fakeDepthbuffer->blitCurrentReadToFakeDraw(target, level, x, y, width, height);
    }
}

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    // LOGI("glTexParameterf: target=%u pname=%u param=%f", target, pname, param);
    FFPE::List::addCommand<OV_glTexParameterf>(target, pname, param);

    selectProperTexParamf(target, pname, param);
    glTexParameterf(target, pname, param);
}

void OV_glTexParameteri(GLenum target, GLenum pname, GLint param) {
    // LOGI("glTexParameteri: target=%u pname=%u param=%d", target, pname, param);
    FFPE::List::addCommand<OV_glTexParameteri>(target, pname, param);

    // selectProperTexParami(target, pname, param);
    glTexParameteri(target, pname, param);
}
