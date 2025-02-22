#include "gles30/main.h"
#include "gles30/translation.h"
#include "utils/log.h"

void GLES30::GLES30Wrapper::init() {
    LOGI("GLES 3.x overrides entrypoint!");
    GLES30::registerTranslatedFunctions();
}