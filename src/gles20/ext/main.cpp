#include "gles20/ext/main.h"
#include "utils/log.h"

void GLES20Ext::GLES20ExtWrapper::init() {
    LOGI("GLES 2.0 extension entrypoint!");

    GLES20Ext::register_EXT_buffer_storage();
    GLES20Ext::register_OES_mapbuffer();
    GLES20Ext::register_EXT_blend_func_extended();
}