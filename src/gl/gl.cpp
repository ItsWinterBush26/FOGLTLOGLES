#include "gl/gl.h"
#include "utils/log.h"

FunctionPtr glXGetProcAddress(const char* pn) {
    LOGI("LWJGL asking for %s", pn);
    return getFunctionAddress(pn);
}