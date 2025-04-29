#include "gl/gl.h"
#include "egl/egl.h"
#include "main.h"
#include "utils/log.h"

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

void initDebug() {
    REGISTEROV(glDebugMessageCallback);
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
