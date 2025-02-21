#include "gl/gl.h"
#include "main.h"
#include <GLES2/gl2.h>
#include <mutex>

static std::once_flag glInitFlag;

FunctionPtr glXGetProcAddress(const char* pn) {
    // std::call_once(glInitFlag, glInit);
    return FOGLTLOGLES::getFunctionAddress(pn);
}

void glInit() { }