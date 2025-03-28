// Based on:
// https://github.com/artdeell/LTW/blob/master/ltw/src/main/tinywrapper/framebuffer.c

#include "es/framebuffer.h"
#include "es/state_tracking.h"
#include "es/utils.h"
#include "gles32/main.h"
#include "main.h"

#include <GLES3/gl3.h>

void glDrawBuffer(GLenum buffer);
void OV_glDrawBuffers(GLsizei n, const GLenum* buffers);

void OV_glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value);
void OV_glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value);
void OV_glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value);

void OV_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void OV_glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void OV_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

void OV_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
GLenum OV_glCheckFramebufferStatus(GLenum target);

void GLES32::registerFramebufferOverrides() {
    REGISTER(glDrawBuffer);
    REGISTEROV(glDrawBuffers);

    REGISTEROV(glClearBufferfv);
    REGISTEROV(glClearBufferiv)
    REGISTEROV(glClearBufferuiv);

    REGISTEROV(glFramebufferTexture2D);
    REGISTEROV(glFramebufferTextureLayer);
    REGISTEROV(glFramebufferRenderbuffer);

    REGISTEROV(glGetFramebufferAttachmentParameteriv);
    REGISTEROV(glCheckFramebufferStatus);

    fakeDepthbuffer = std::make_shared<FakeDepthFramebuffer>();
}

void glDrawBuffer(GLenum buffer) {
    OV_glDrawBuffers(1, &buffer);
}

void OV_glDrawBuffers(GLsizei n, const GLenum* buffers) {
    auto framebuffer = getFramebufferObject(GL_DRAW_FRAMEBUFFER);
    if (framebuffer.get() == nullptr) {
        glDrawBuffers(n, buffers);
        return;
    }

    framebuffer->bufferAmount = n;
    std::memcpy(framebuffer->virtualDrawbuffers, buffers, n * sizeof(GLenum));

    // no bounds checking as theres a memcpy
    // above that clearly ignores MAX_DRAWBUFFERS
    for (int i = 0; i < n; ++i) {
        GLenum buffer = buffers[i];
        rebindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer, buffer);

        if (buffer != GL_NONE) framebuffer->physicalDrawbuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        else framebuffer->physicalDrawbuffers[i] = GL_NONE;
    }

    glDrawBuffers(n, framebuffer->physicalDrawbuffers);
}

void OV_glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value) {
    clearFramebuffer(buffer, drawbuffer);
    glClearBufferfv(buffer, drawbuffer, value);
}

void OV_glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value) {
    clearFramebuffer(buffer, drawbuffer);
    glClearBufferiv(buffer, drawbuffer, value);
}

void OV_glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value) {
    clearFramebuffer(buffer, drawbuffer);
    glClearBufferuiv(buffer, drawbuffer, value);
}


void OV_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    auto framebuffer = getFramebufferObject(target);
    GLuint attachmentIndex = getAttachmentIndex(attachment);

    if (framebuffer.get() == nullptr || attachmentIndex == -1) {
        glFramebufferTexture2D(target, attachment, textarget, texture, level);
        return;
    }

    if (texture == 0) {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = GL_NONE;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = 0;
        framebuffer->colorInfo.colorLevels[attachmentIndex] = 0;
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = 0;
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = 0;
    } else {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = textarget;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = texture;
        framebuffer->colorInfo.colorLevels[attachmentIndex] = level;

        GLenum textureFormat = trackedStates->activeTextureState->textureInternalFormats[texture];
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = getComponentTypeFromFormat(textureFormat);
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = isSRGBFormat(textureFormat) ? GL_SRGB : GL_LINEAR;
    }

    rebindFramebuffer(target, framebuffer, attachment);
}

void OV_glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) {
    auto framebuffer = getFramebufferObject(target);
    GLuint attachmentIndex = getAttachmentIndex(attachment);

    if (framebuffer.get() == nullptr || attachmentIndex == -1) {
        glFramebufferTextureLayer(target, attachment, texture, level, layer);
        return;
    }

    if (texture == 0) {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = GL_NONE;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = 0;
        framebuffer->colorInfo.colorLevels[attachmentIndex] = 0;
        framebuffer->colorInfo.colorLayers[attachmentIndex] = 0;
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = 0;
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = 0;
    } else {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = texture;
        framebuffer->colorInfo.colorLevels[attachmentIndex] = level;
        framebuffer->colorInfo.colorLayers[attachmentIndex] = layer;
        
        GLenum textureFormat = trackedStates->activeTextureState->textureInternalFormats[texture];
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = getComponentTypeFromFormat(textureFormat);
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = isSRGBFormat(textureFormat) ? GL_SRGB : GL_LINEAR;
    }

    rebindFramebuffer(target, framebuffer, attachment);
}

void OV_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    auto framebuffer = getFramebufferObject(target);
    GLuint attachmentIndex = getAttachmentIndex(attachment);

    if (framebuffer.get() == nullptr || attachmentIndex == -1) {
        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
        return;
    }

    if (renderbuffer == 0) {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = GL_NONE;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = 0;
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = 0;
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = 0;
    } else {
        framebuffer->colorInfo.colorTargets[attachmentIndex] = renderbuffertarget;
        framebuffer->colorInfo.colorObjects[attachmentIndex] = renderbuffer;
        
        GLenum renderbufferFormat = trackedStates->renderbufferInternalFormats[renderbuffer];
        framebuffer->colorInfo.colorComponentType[attachmentIndex] = getComponentTypeFromFormat(renderbufferFormat);
        framebuffer->colorInfo.colorEncoding[attachmentIndex] = isSRGBFormat(renderbufferFormat) ? GL_SRGB : GL_LINEAR;
    }

    rebindFramebuffer(target, framebuffer, attachment);
}

void OV_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) {
    auto framebuffer = getFramebufferObject(target);
    GLuint attachmentIndex = getAttachmentIndex(attachment);

    if (framebuffer.get() == nullptr || attachmentIndex == -1) {
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
        return;
    }

    getFramebufferAttachmentParameter(framebuffer, attachmentIndex, pname, params);
}

GLenum OV_glCheckFramebufferStatus(GLenum target) {
    GLenum framebufferStatus = glCheckFramebufferStatus(target);
    if (framebufferStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
        auto framebuffer = getFramebufferObject(target);
        for (GLint i = 0; i < MAX_FBTARGETS; ++i) {
            if (framebuffer->colorInfo.colorTargets[i] != GL_NONE
             || framebuffer->colorInfo.colorObjects[i] != 0) return GL_FRAMEBUFFER_COMPLETE;
        }
    }

    return framebufferStatus;
}
