#include "es/ffp.h"
#include "gles/ffp/enums.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glAlphaFunc(GLenum op, GLclampf threshold);

void FFP::registerAlphaTestFunctions() {
    REGISTER(glAlphaFunc);
}

void glAlphaFunc(GLenum op, GLclampf threshold) {
    if (!trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) return;
    Lists::displayListManager->addCommand<glAlphaFunc>(op, threshold);

    FFPE::States::AlphaTest::op = op;
    FFPE::States::AlphaTest::threshold = threshold;
}
