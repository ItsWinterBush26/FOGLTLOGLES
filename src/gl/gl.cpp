#include "gl/gl.h"
#include "main.h"

FunctionPtr glXGetProcAddress(const char* pn) {
    return FOGLTLOGLES::getFunctionAddress(pn);
}