#include "gl/gl.h"
#include "main.h"

FunctionPtr glXGetProcAddress(const GLchar* pn) {
    return FOGLTLOGLES::getFunctionAddress(pn);
}