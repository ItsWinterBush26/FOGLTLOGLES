#include "egl/egl.h"
#include "es/utils.h"
#include "gles20/ext/main.h"
#include "main.h"

void GLES20Ext::register_EXT_buffer_storage() {
    if (!ESUtils::isExtensionSupported("GL_EXT_buffer_storage")) return;
    LOGI("EXT_buffer_storage is present and so used.");

    ESUtils::fakeExtensions.insert("GL_ARB_buffer_storage");

    REGISTEREXT(glBufferStorage, "EXT");
}
