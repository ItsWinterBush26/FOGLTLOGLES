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
    GLenum virtualDrawbuffers[MAX_DRAWBUFFERS];
    GLenum physicalDrawbuffers[MAX_DRAWBUFFERS];
    GLsizei nbuffers;
} Framebuffer;

inline std::unordered_map<GLuint, Framebuffer> boundFramebuffers;

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

void OV_glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    glGenFramebuffers(n, framebuffers);

    for (GLsizei i = 0; i < n; ++i) {
        
    }
}