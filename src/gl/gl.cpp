#include "gl/gl.hpp"
#include "egl/egl.hpp"
#include "main.hpp"
#include "utils/env.hpp"
#include "utils/log.hpp"

#include <mutex>
#include <GLES3/gl32.h>

inline std::once_flag initDebugFlag;

void OV_glDebugMessageCallback(GLDEBUGPROC callback, const void* userParam);

FunctionPtr glXGetProcAddress(const GLchar* pn) {
    std::call_once(eglInitFlag, eglInit);
    std::call_once(rendererInitFlag, FOGLTLOGLES::init);
    std::call_once(initDebugFlag, initDebug);

    FunctionPtr tmp = FOGLTLOGLES::getFunctionAddress(pn);
    
    if (tmp) return tmp;
    if (real_eglGetProcAddress) return real_eglGetProcAddress(pn);

    return nullptr;
}

void customDebugCallback(
    GLenum, GLenum,
    GLuint, GLenum,
    GLsizei, const GLchar*,
    const void*
);

void initDebug() {
    if (getEnvironmentVar("LIBGL_VGPU_DUMP", "0") == "1") {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(customDebugCallback, nullptr);
    }

    // REGISTEROV(glDebugMessageCallback);
}

void customDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    LOGD("[GL DEBUG] Source: %u, Type: %u, ID: %u, Severity: %u", source, type, id, severity);
    LOGD("[GL DEBUG] Message: %s", message);
}

void OV_glDebugMessageCallback(GLDEBUGPROC callback, const void* userParam) {
    LOGI("Intercepted glDebugMessageCallback!");
    glDebugMessageCallback(customDebugCallback, userParam);
}
