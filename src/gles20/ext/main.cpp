#include "gles20/ext/main.h"
#include "utils/log.h"

void GLES20Ext::GLES20ExtWrapper::init() {
    LOGI("GLES2.0 extension entrypoint!");

    GLES20Ext::register_ARB_buffer_storage();
}