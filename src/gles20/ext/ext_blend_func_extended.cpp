#include "egl/egl.h"
#include "es/utils.h"
#include "gles20/ext/main.h"
#include "main.h"

void GLES20Ext::register_EXT_blend_func_extended() {
    if (!ESUtils::isExtensionSupported("GL_EXT_blend_func_extended")) return;
    ESUtils::fakeExtensions.insert("ARB_blend_func_extended");

    REGISTEREXT(glBindFragDataLocation, "EXT");
    REGISTEREXT(glBindFragDataLocationIndexed, "EXT");
}