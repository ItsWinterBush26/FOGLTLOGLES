#include "gles30/main.h"
#include "utils/log.h"

void GLES30::GLES30Wrapper::init() {
    LOGI("GLES 3.x overrides entrypoint!");

    GLES30::registerMappableFunctions();
    GLES30::registerTranslatedFunctions();
}