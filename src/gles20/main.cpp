#include "gles20/main.h"
#include "gles20/extensions.h"
#include "gles20/mappable.h"
#include "gles20/translation.h"

#include <GLES2/gl2.h>

void GLES20::GLES20Wrapper::init() {
    GLES20::registerMappableFunctions();
    GLES20::registerTranslatedFunctions();
    GLES20::registerExtensionFunctions();
}