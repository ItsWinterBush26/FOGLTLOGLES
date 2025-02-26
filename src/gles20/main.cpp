#include "gles20/main.h"
#include "gles20/extensions.h"
#include "gles20/mappable.h"
#include "gles20/translation.h"
#include "utils/log.h"

void GLES20::GLES20Wrapper::init() {
    LOGI("GLES 2.x overrides entrypoint!");

    GLES20::registerMappableFunctions();
    GLES20::registerShaderOverrides();
    GLES20::registerTranslatedFunctions();
    GLES20::registerExtensionFunctions();
}