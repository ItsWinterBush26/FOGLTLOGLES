#include "gles/main.h"
#include "utils/log.h"

void GLES::GLESWrapper::init() {
    LOGI("GLES 1.0 entrypoint!");

    GLES::registerTranslatedFunctions();
    GLES::registerBrandingOverride();
    // GLES::registerNoErrorOverride();
}