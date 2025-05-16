#include "gles/main.h"
#include "main.h"
#include "utils/env.h"

#include <GLES3/gl32.h>

inline bool envNoError = (getEnvironmentVar("LIBGL_NO_ERROR") == "1");

GLenum OV_glGetError();

void GLES::registerNoErrorOverride() {
    // REGISTEROV(glGetError);
}

GLenum OV_glGetError() {
    if (envNoError) return GL_NO_ERROR;
    return glGetError();
}