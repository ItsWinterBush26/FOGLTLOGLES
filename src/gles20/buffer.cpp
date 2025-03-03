#include "gles20/main.h"

#include <GLES2/gl2.h>

#include "gl/header.h"
#include "main.h"

void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);

void GLES20::registerBufferWorkarounds() {
    REGISTER(glBufferStorage);
}

void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) {
    glBufferData(target, size, data, flags & GL_DYNAMIC_STORAGE_BIT ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}