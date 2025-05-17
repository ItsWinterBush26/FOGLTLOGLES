#include "es/ffp.h"
#include "gles/ffp/enums.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glAlphaFunc(GLenum op, GLclampf threshold);

void FFP::registerAlphaTestFunction() {
    REGISTER(glAlphaFunc);
}

void glAlphaFunc(GLenum op, GLclampf threshold) {
    if (!trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) return;

    FFPStates::AlphaTest::op = op;
    FFPStates::AlphaTest::threshold = threshold;
}
