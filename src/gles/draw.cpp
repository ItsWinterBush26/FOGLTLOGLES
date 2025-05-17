#include "gles/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>

void OV_glDrawArrays(GLenum mode, GLint first, GLsizei count);

void GLES::registerDrawOverride() {
    REGISTEROV(glDrawArrays);
}

void OV_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    LOGI("OV_glDrawArrays : mode=%u", mode);

    glDrawArrays(mode, first, count);
}