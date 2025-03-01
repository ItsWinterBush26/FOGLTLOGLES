#include "gles20/main.h"
#include "main.h"

#include <GLES2/gl2.h>

// TODO: texture.cpp shader.cpp separation

void glClearDepth(double d);

void GLES20::registerTranslatedFunctions() {
    REGISTER(glClearDepth);
}

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}
