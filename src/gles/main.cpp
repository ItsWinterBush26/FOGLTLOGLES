#include "gles/main.hpp"
#include "gles/ffp/main.hpp"
#include "es/limits.hpp"
#include "utils/log.hpp"

void GLES::GLESWrapper::init() {
    LOGI("GLES 1.0 entrypoint!");

    GLES::registerTranslatedFunctions();
    GLES::registerBrandingOverride();
    GLES::registerNoErrorOverride();
    GLES::registerDrawOverride();

    ESLimits::init();
    FFP::wrapper->init();
}
