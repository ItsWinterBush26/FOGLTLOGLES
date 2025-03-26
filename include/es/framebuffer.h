// Based on:
// https://github.com/artdeell/LTW/blob/master/ltw/src/main/tinywrapper/framebuffer.c

#pragma once

#include "es/binding_saver.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <memory>
#include <unordered_map>

#ifndef MAX_FBTARGETS
#define MAX_FBTARGETS 8
#endif

#ifndef MAX_DRAWBUFFERS
#define MAX_DRAWBUFFERS 8
#endif

inline GLuint currentDrawFramebuffer = 0, currentReadFramebuffer = 0;

class FakeDepthFramebuffer {
public:
    GLuint drawFramebuffer;
    GLuint readFramebuffer;

    GLuint framebufferTexture;

    GLsizei width, height;
    GLvoid* data;

    bool ready;

    FakeDepthFramebuffer() {
        glGenTextures(1, &framebufferTexture);
        glGenFramebuffers(1, &drawFramebuffer);
        glGenFramebuffers(1, &readFramebuffer);

        SaveBoundedTexture sbt(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        ready = (GET_OVFUNC(PFNGLGETERRORPROC, glGetError)() == GL_NO_ERROR);
    }

    void store(GLint x, GLint y, GLsizei w, GLsizei h) {
        if (!ready) return;

        SaveBoundedTexture sbt(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, framebufferTexture, 0
        );

        glBlitFramebuffer(x, y, x + w, y + h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentDrawFramebuffer);
    }

    void release(GLenum target, GLint level, GLint x, GLint y, GLsizei w, GLsizei h) {
        if (!ready) return;

        GLint boundTexture;
        glGetIntegerv(getBindingEnumOfTexture(target), &boundTexture);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);

        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            target, boundTexture, level
        );

        glBlitFramebuffer(
            0, 0,
            w, h,
            x, y,
            x + w, y + h,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentDrawFramebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, currentReadFramebuffer);
    }
};

struct FramebufferColorInfo {
    GLuint colorTargets[MAX_FBTARGETS];
    GLuint colorObjects[MAX_FBTARGETS];
    GLuint colorComponentType[MAX_FBTARGETS];
    GLuint colorEncoding[MAX_FBTARGETS];

    // if GL_TEXTURE
    GLuint colorLevels[MAX_FBTARGETS];
    GLuint colorLayers[MAX_FBTARGETS];
};

struct Framebuffer {
    FramebufferColorInfo colorInfo;
    GLenum virtualDrawbuffers[MAX_DRAWBUFFERS] = { GL_COLOR_ATTACHMENT0 };
    GLenum physicalDrawbuffers[MAX_DRAWBUFFERS];
    GLsizei bufferAmount = 1;
};

inline std::shared_ptr<FakeDepthFramebuffer> fakeDepthbuffer;
inline std::unordered_map<GLuint, std::shared_ptr<Framebuffer>> boundFramebuffers;

inline std::shared_ptr<Framebuffer> getFramebufferObject(GLenum target) {
    GLuint framebuffer = 0;
    switch (target) {
        case GL_FRAMEBUFFER:
        case GL_DRAW_FRAMEBUFFER:
            framebuffer = currentDrawFramebuffer;
            break;
        case GL_READ_FRAMEBUFFER:
            framebuffer = currentReadFramebuffer;
            break;
    }
    
    return boundFramebuffers[framebuffer];
}

inline GLuint getAttachmentIndex(GLenum attachment) {
    switch (attachment) {
        case GL_NONE:
        case GL_DEPTH_ATTACHMENT:
        case GL_STENCIL_ATTACHMENT:
        case GL_DEPTH_STENCIL_ATTACHMENT:
            return -1;
    }

    return attachment - GL_COLOR_ATTACHMENT0;
}

inline GLenum getAttachmentObjectType(GLenum framebufferTarget) {
    switch (framebufferTarget) {
        case GL_NONE: return GL_NONE;
        case GL_RENDERBUFFER: return GL_RENDERBUFFER;
        default: return GL_TEXTURE;
    }
}

inline GLenum getMapAttachment(std::shared_ptr<Framebuffer> framebuffer, GLenum attachment) {
    if (framebuffer->bufferAmount == 0) return GL_NONE;

    for (GLsizei i = 0; i < framebuffer->bufferAmount; ++i) {
        if (framebuffer->virtualDrawbuffers[i] == attachment) {
            return attachment;
        }
    }

    return GL_NONE;
}

inline void rebindFramebuffer(GLenum target, std::shared_ptr<Framebuffer> framebuffer, GLenum virtualAttachment) {
    GLuint virtualIndex = getAttachmentIndex(virtualAttachment);
    if (virtualIndex == -1) return;

    GLenum physicalAttachment = getMapAttachment(framebuffer, virtualAttachment);
    if (physicalAttachment == GL_NONE) return;

    switch (framebuffer->colorInfo.colorTargets[virtualIndex]) {
        case GL_NONE:
            glFramebufferRenderbuffer(
                target, 
                physicalAttachment, 
                GL_RENDERBUFFER, 
                0
            );
            break;
        case GL_RENDERBUFFER:
            glFramebufferRenderbuffer(
                target, 
                physicalAttachment, 
                GL_RENDERBUFFER, 
                framebuffer->colorInfo.colorObjects[virtualIndex]
            );
            break;
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
            glFramebufferTextureLayer(
                target, 
                physicalAttachment,
                framebuffer->colorInfo.colorObjects[virtualIndex],
                framebuffer->colorInfo.colorLevels[virtualIndex],
                framebuffer->colorInfo.colorLayers[virtualIndex]
            );
            break;
        default:
            glFramebufferTexture2D(
                target, 
                physicalAttachment,
                framebuffer->colorInfo.colorTargets[virtualIndex],
                framebuffer->colorInfo.colorObjects[virtualIndex],
                framebuffer->colorInfo.colorLevels[virtualIndex]
            );
            break;
    }
}

inline void getFramebufferAttachmentParameter(std::shared_ptr<Framebuffer> framebuffer, GLuint attachmentIndex, GLenum pname, GLint* params) {
    GLenum framebufferTarget = framebuffer->colorInfo.colorTargets[attachmentIndex];
    GLenum targetObjectType = getAttachmentObjectType(framebufferTarget);
    
    switch (pname) {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
            (*params) = targetObjectType;
            break;
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
            if (targetObjectType == GL_RENDERBUFFER
             || targetObjectType == GL_TEXTURE) (*params) = framebuffer->colorInfo.colorObjects[attachmentIndex];
            else (*params) = 0;
            break;

        // attachment object type is not NONE
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
            if (targetObjectType == GL_NONE) (*params) = 0;
            else (*params) = framebuffer->colorInfo.colorComponentType[attachmentIndex];
            break;

        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
            if (targetObjectType == GL_NONE) (*params) = 0;
            else (*params) = framebuffer->colorInfo.colorEncoding[attachmentIndex];
            break;

        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
            LOGI("GL_FRAMEBUFFER_ATTACHMENT_*_SIZE are not yet implemented.");
            break;

        // attachment object type is a texture
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
            if (targetObjectType == GL_TEXTURE) (*params) = framebuffer->colorInfo.colorLevels[attachmentIndex];
            else (*params) = 0;
            break;
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
            if (framebufferTarget == GL_TEXTURE) (*params) = framebuffer->colorInfo.colorLayers[attachmentIndex];
            else (*params) = 0;
            break;
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
        if (targetObjectType != GL_TEXTURE
         || framebufferTarget < GL_TEXTURE_CUBE_MAP_POSITIVE_X
         || framebufferTarget > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
         || framebuffer->colorInfo.colorObjects[attachmentIndex] != GL_TEXTURE_CUBE_MAP) (*params) = 0;
        else (*params) = framebufferTarget;
        break;
    }
}

inline void clearFramebuffer(GLenum buffer, GLint& drawBuffer) {
    auto framebuffer = getFramebufferObject(GL_DRAW_FRAMEBUFFER);
    if (framebuffer.get() != nullptr && buffer == GL_COLOR) {
        GLenum attachment = getMapAttachment(framebuffer, GL_COLOR_ATTACHMENT0 + drawBuffer);
        drawBuffer = attachment - GL_COLOR_ATTACHMENT0;
    }
}
