#include "gles/main.h"
#include "main.h"

#include <GLES/gl.h>

void glClearDepth(double d);
void glDepthRange(double near, double far);

void GLES::registerTranslatedFunctions() {
    REGISTER(glClearDepth);
    REGISTER(glDepthRange);
}

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

void glDepthRange(double near, double far) {
    glDepthRangef(static_cast<float>(near), static_cast<float>(far));
}