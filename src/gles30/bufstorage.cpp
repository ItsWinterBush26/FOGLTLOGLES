#include "gles30/main.h"
#include "main.h"
#include "utils/log.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

// void glBufferStorage(GLenum target, GLsizeiptr size, const GLvoid* data, GLbitfield flags);

inline PFNGLBUFFERSTORAGEEXTPROC real_glBufferStorage;

void GLES30::registerBufferStorageFunction() {
    real_glBufferStorage = reinterpret_cast<PFNGLBUFFERSTORAGEEXTPROC>(eglGetProcAddress("glBufferStorage"));

    if (real_glBufferStorage) LOGI("glBufferStorage present!");

    FOGLTLOGLES::registerFunction("glBufferStorage", TO_FUNCPTR(real_glBufferStorage));
}

/* void glBufferStorage(
    GLenum target,
    GLsizeiptr size,
    const GLvoid* data,
    GLbitfield flags
) {

}    */