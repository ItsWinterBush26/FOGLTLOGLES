#include "es/framebuffer.h"
#include "gles20/main.h"

#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <unordered_map>

#define MAX_FBTARGETS 8
#define MAX_DRAWBUFFERS 8

typedef struct {
    GLuint colorTargets[MAX_FBTARGETS];
    GLuint colorObjects[MAX_FBTARGETS];
    GLuint colorLevels[MAX_FBTARGETS];
    GLuint colorLayers[MAX_FBTARGETS];
} FramebufferColorInfo;

typedef struct {
    FramebufferColorInfo colorInfo;
    GLenum virtualDrawbuffers[MAX_DRAWBUFFERS] = { GL_COLOR_ATTACHMENT0 };
    GLenum physicalDrawbuffers[MAX_DRAWBUFFERS];
    GLsizei bufferAmount = 1;
} Framebuffer;

inline GLuint drawBuffer = 0, readBuffer = 0;
inline std::unordered_map<GLuint, std::shared_ptr<Framebuffer>> boundFramebuffers;

std::shared_ptr<Framebuffer> getFramebufferObject(GLenum target) {
    GLuint framebuffer = 0;
    switch (target) {
        case GL_FRAMEBUFFER:
        case GL_DRAW_FRAMEBUFFER:
            framebuffer = drawBuffer;
            break;
        case GL_READ_FRAMEBUFFER:
            framebuffer = readBuffer;
            break;
    }

    return boundFramebuffers.at(framebuffer);
}

inline GLenum getMapAttachment(std::shared_ptr<Framebuffer> framebuffer, GLenum attachment) {
    if (framebuffer->bufferAmount == 0) return GL_NONE;

    for (GLsizei i = 0; i < framebuffer->bufferAmount; ++i) {
        if (framebuffer->virtualDrawbuffers[i] == attachment) {
            return framebuffer->virtualDrawbuffers[i];
        }
    }

    return GL_NONE;
}

void OV_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
void OV_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void OV_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void OV_glBindFramebuffer(GLenum target, GLuint framebuffer);

void OV_glDrawBuffers(GLsizei n, const GLenum* buffers);
void glDrawBuffer(GLenum buffer) { OV_glDrawBuffers(1, &buffer); }

GLenum OV_glCheckFramebufferStatus(GLenum target);
void OV_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params);

void OV_glClearBufferiv(GLenum pname, GLint drawbuffer, const GLint* value);
void OV_glClearBufferuiv(GLenum pname, GLint drawbuffer, const GLint* value);
void OV_glClearBufferfv(GLenum pname, GLint drawbuffer, const GLint* value);

void GLES20::registerFramebufferOverride() {
    
}

void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params) {
    
}

void OV_glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    glGenFramebuffers(n, framebuffers);

    Framebuffer* framebuffer;
    for (GLsizei i = 0; i < n; ++i) {
        boundFramebuffers.insert({ framebuffers[i], std::make_shared<Framebuffer>() });
    }
}

void OV_glBindFramebuffer(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);

    switch (target) {
        case GL_FRAMEBUFFER:
            readBuffer = drawBuffer = framebuffer;
            break;
        case GL_READ_FRAMEBUFFER:
            readBuffer = framebuffer;
            break;
        case GL_DRAW_FRAMEBUFFER:
            drawBuffer = framebuffer;
            break;
    }
}