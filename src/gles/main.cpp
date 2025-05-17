#include "gles/main.h"
#include "gles/ffp/main.h"
#include "es/limits.h"
#include "utils/log.h"

void GLES::GLESWrapper::init() {
    LOGI("GLES 1.0 entrypoint!");

    GLES::registerTranslatedFunctions();
    GLES::registerBrandingOverride();
    GLES::registerNoErrorOverride();
    GLES::registerDrawOverride();

    ESLimits::init();
    FFP::wrapper->init();
}
