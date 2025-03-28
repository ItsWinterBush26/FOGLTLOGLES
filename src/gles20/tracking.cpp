#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "gles20/framebuffer_tracking.h"
#include "gles20/main.h"
#include "gles20/texture_tracking.h"
#include "gles20/renderbuffer_tracking.h"
#include "main.h"

#include <GLES2/gl2.h>
#include <utility>

void GLES20::registerTrackingFunctions() {
    REGISTEROV(glBindFramebuffer);

    REGISTEROV(glBindTexture);
    REGISTEROV(glDeleteTextures); // deleting tracked iformat
    REGISTEROV(glActiveTexture);

    REGISTEROV(glBindBuffer);

    REGISTEROV(glBindRenderbuffer);
    REGISTEROV(glDeleteRenderbuffers); // deleting tracked iformat
}

void OV_glBindFramebuffer(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);

    switch (target) {
        case GL_FRAMEBUFFER:
            trackedStates->boundReadFramebuffer = trackedStates->boundDrawFramebuffer = framebuffer;
            break;
        case GL_READ_FRAMEBUFFER: trackedStates->boundReadFramebuffer = framebuffer; break;
        case GL_DRAW_FRAMEBUFFER: trackedStates->boundDrawFramebuffer = framebuffer; break;
    }

    trackedStates->recentlyBoundFramebuffer = std::make_pair(target, framebuffer);
}

void OV_glBindTexture(GLenum target, GLuint texture) {
    glBindTexture(target, texture);

    trackedStates->textureUnits[trackedStates->activeTextureUnit].boundTextures[target] = texture;
}

void OV_glDeleteTextures(GLsizei n, const GLuint *textures) {
    glDeleteTextures(n, textures);

    for (GLsizei i = 0; i < n; ++i) {
        trackedStates->textureUnits[trackedStates->activeTextureUnit].textureInternalFormats.erase(textures[i]);
    }
}

void OV_glActiveTexture(GLuint texture) {
    glActiveTexture(texture);

    trackedStates->activeTextureUnit = texture;
}

void OV_glBindBuffer(GLenum target, GLuint buffer) {
    glBindBuffer(target, buffer);

    trackedStates->boundBuffers[target] = buffer;
}

void OV_glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    glBindRenderbuffer(target, renderbuffer);

    trackedStates->boundRenderbuffer = renderbuffer;
}

void OV_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    glDeleteRenderbuffers(n, renderbuffers);

    for (GLint i = 0; i < n; ++i) {
        trackedStates->renderbufferInternalFormats.erase(renderbuffers[i]);
    }
}