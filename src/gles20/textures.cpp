#include "es/framebuffer.h"
#include "es/proxy.h"
#include "es/state_tracking.h"
#include "es/swizzling.h"
#include "es/texture.h"
#include "es/texparam.h"
#include "gles20/main.h"
#include "main.h"
#include "utils/log.h"
#include "utils/pointers.h"

#include <GLES2/gl2.h>

void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
void OV_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void OV_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

void OV_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void OV_glTexParameteri(GLenum target, GLenum pname, GLint param);

inline GLint maxTextureSize = 0;

void GLES20::registerTextureOverrides() {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOGI("GL_MAX_TEXTURE_SIZE is %i", maxTextureSize);

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

void OV_glTexSubImage2D(
    GLenum target, GLint level, 
    GLint xOffset, GLint yOffset, 
    GLsizei width, GLsizei height, 
    GLenum format, GLenum type,
    const void* pixels
) {
    std::vector<SwizzleOperation> ops;
    swizzleBGRA(type, ops);
    doSwizzling(target, ops);

    if (isDepthFormat(format)) {
        if (width == fakeDepthbuffer->width && height == fakeDepthbuffer->height
            && fakeDepthbuffer->data == pixels) {
            fakeDepthbuffer->blitFakeReadToFakeDraw(target, level, xOffset, yOffset, width, height);
            return;
        }
    }

    glTexSubImage2D(
        target, level,
        xOffset, yOffset,
        width, height,
        format, type,
        pixels
    );
}

void OV_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
    if (isDepthFormat(internalformat)) {
        OV_glTexImage2D(
            target, level, trackedStates->textureUnits[trackedStates->activeTextureUnit].textureInternalFormats[trackedStates->textureUnits[trackedStates->activeTextureUnit].boundTextures[GL_TEXTURE_2D]],
            width, height, border,
            GL_DEPTH_COMPONENT32F, GL_FLOAT,
            nullptr
        );

        fakeDepthbuffer->blitCurrentReadToFakeDraw(target, level, x, y, width, height);
    } else {
        glCopyTexImage2D(
            target,  level, internalformat,
            x, y,
            width, height, border
        );
    }
}

void OV_glCopyTexSubImage2D(
    GLenum target, GLint level,
    GLint xoffset, GLint yoffset,
    GLint x, GLint y,
    GLsizei width, GLsizei height
) {
    /* LOGI("glCopyTexSubImage2D: target=%u level=%i xoff=%i yoff=%i x=%i y=%i w=%u h=%u",
        target, level,
        xoffset, yoffset,
        x, y, width, height
    ); */
    
    if (isDepthFormat(trackedStates->textureUnits[trackedStates->activeTextureUnit].textureInternalFormats[trackedStates->textureUnits[trackedStates->activeTextureUnit].boundTextures[GL_TEXTURE_2D]])) {
        // LOGI("Bound image is a depth texture, re-routing to FakeDepthFramebuffer");
        fakeDepthbuffer->blitCurrentReadToFakeDraw(target, level, x, y, width, height);
    } else {
        glGetError();
        glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
        if (glGetError() == GL_INVALID_OPERATION) {
            // LOGI("glCopyTexSubImage2D failed with GL_INVALID_OPERATION. Re-routing to FakeDepthFramebuffer");
        
            fakeDepthbuffer->blitCurrentReadToFakeDraw(target, level, x, y, width, height);
        }
    }
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
