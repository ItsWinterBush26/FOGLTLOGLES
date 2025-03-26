#include "gl/gl.h"
#include "main.h"
#include "utils/log.h"

#include <mutex>
#include <GLES3/gl32.h>
#include <utility>

inline std::once_flag initDebugFlag;

void OV_glDebugMessageCallback(GLDEBUGPROC callback, const void* userParam);

FunctionPtr glXGetProcAddress(const GLchar* pn) {
    std::call_once(initDebugFlag, initDebug);
    
    return FOGLTLOGLES::getFunctionAddress(pn);
}

void initDebug() {
    // REGISTEROV(glDebugMessageCallback);
}

inline std::pair<GLDEBUGPROC, const void*> previousRegisteredCallback;

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

    previousRegisteredCallback.first(
        source, type, id, severity, length, message, previousRegisteredCallback.second
    );
}

void OV_glDebugMessageCallback(GLDEBUGPROC callback, const void* userParam) {
    LOGI("Intercepted glDebugMessageCallback!");
    previousRegisteredCallback = std::make_pair(callback, userParam);

    callback = customDebugCallback;
    glDebugMessageCallback(callback, userParam);
}