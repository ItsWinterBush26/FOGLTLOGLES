#include "gles20/translation.h"
#include "main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>


void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

void GLES20::registerTranslatedFunctions() {
    LOGI("Hi from translation.cpp!");

    REGISTER(glClearDepth);
}