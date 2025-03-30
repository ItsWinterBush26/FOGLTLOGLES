#include "egl/egl.h"
#include "es/utils.h"
#include "gles20/ext/main.h"
#include "main.h"

void GLES20Ext::register_EXT_draw_elements_base_vertex() {
    if (!ESUtils::isExtensionSupported("OES_draw_elements_base_vertex")) return;

    REGISTEREXT(glMultiDrawElementsBaseVertex, "EXT");
}