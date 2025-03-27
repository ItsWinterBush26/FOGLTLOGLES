#include "es/renderbuffer.h"
#include "gles20/main.h"
#include "main.h"

#include <GLES2/gl2.h>

void OV_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

void GLES20::registerRenderbufferWorkaround() {
    REGISTEROV(glRenderbufferStorage);
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
