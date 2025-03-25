#include "gles30/main.h"
#include "utils/log.h"

void GLES30::GLES30Wrapper::init() {
    LOGI("GLES 3.0 entrypoint!");

    GLES30::registerTranslatedFunctions();
    // GLES30::registerDSAEmulation();
    GLES30::registerTextureOverrides();
    GLES30::registerMultiDrawEmulation();
}