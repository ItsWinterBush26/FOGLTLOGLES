#include "gles/main.h"
#include "utils/log.h"

void GLES::GLESWrapper::init() {
    LOGI("GLES 1.0 endpoint!");

    GLES::registerBrandingOverride();
}