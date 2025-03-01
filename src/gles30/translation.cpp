#include "gles30/main.h"
#include "es/texparam.h"
#include "gl/glext.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl3.h>

void* glMapBuffer(GLenum target, GLenum access);

void GLES30::registerTranslatedFunctions() {
    REGISTER(glMapBuffer);
}

void* glMapBuffer(GLenum target, GLenum access) {
    GLenum accessRange;
    GLint bufferSize;

    switch (target) {
        case GL_QUERY_BUFFER: // 4.4
        
        case GL_DISPATCH_INDIRECT_BUFFER: // 4.3
        case GL_SHADER_STORAGE_BUFFER: // 4.3

        case GL_ATOMIC_COUNTER_BUFFER: // 4.2

        case GL_DRAW_INDIRECT_BUFFER: // 4.0

        case GL_TEXTURE_BUFFER: // 3.1
            LOGI("glMapBuffer unsupported/unimplemented target=0x%x", target);
        break;
    }

    switch (access) {
        case GL_READ_ONLY:
            accessRange = GL_MAP_READ_BIT;
        break;

        case GL_WRITE_ONLY:
            accessRange = GL_MAP_WRITE_BIT;
        break;

        case GL_READ_WRITE:
            accessRange = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
        break;
    }

    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
    return glMapBufferRange(target, 0, bufferSize, accessRange);
}
