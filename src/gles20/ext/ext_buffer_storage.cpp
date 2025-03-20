#include "es/utils.h"
#include "gles20/ext/main.h"
#include "main.h"

#include <EGL/egl.h>

void GLES20Ext::register_EXT_buffer_storage() {
    if (!ESUtils::isExtensionSupported("GL_EXT_buffer_storage")) return;
    ESUtils::fakeExtensions.insert("GL_ARB_buffer_storage");

    REGISTEREXT(glBufferStorage, "EXT");
}
