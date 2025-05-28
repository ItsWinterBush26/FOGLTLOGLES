#include "es/utils.h"
#include "gles20/ext/main.h"
#include "utils/log.h"

void GLES20Ext::register_GL_ARB_compute_shader() {
    LOGI("Exposing GL_ARB_compute_shader.");

    ESUtils::fakeExtensions.insert("GL_ARB_compute_shader");
}
