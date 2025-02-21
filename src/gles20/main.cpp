#include "gles20/main.h"
#include "gles20/mappable.h"
#include "gles20/translation.h"

#include <GLES2/gl2.h>

void GLES20::GLES20Wrapper::init() {
    GLES20::initTranslations();
    GLES20::initMappableFunctions();
}