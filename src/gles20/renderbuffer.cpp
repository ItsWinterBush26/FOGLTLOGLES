#include "es/renderbuffer.h"
#include "gles20/main.h"
#include "main.h"

#include <GLES2/gl2.h>

void OV_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void OV_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);

void GLES20::registerRenderbufferWorkaround() {
    REGISTEROV(glRenderbufferStorage);
    REGISTEROV(glDeleteRenderbuffers);
}

void OV_glRenderbufferStorage(
    GLenum target,
    GLenum internalformat,
    GLsizei width,
    GLsizei height
) {
    fixStorageParams(internalformat);
    glRenderbufferStorage(target, internalformat, width, height);

    trackRenderbufferFormat(internalformat);
}

void OV_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    glDeleteRenderbuffers(n, renderbuffers);

    for (GLint i = 0; i < n; ++i) {
        trackedRenderbuffers.erase(renderbuffers[i]);
    }
}