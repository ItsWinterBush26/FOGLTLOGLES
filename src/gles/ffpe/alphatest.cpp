#include "es/ffp.h"
#include "es/state_tracking.h"
#include "gles/ffp/enums.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glAlphaFunc(GLenum op, GLclampf threshold);

void FFP::registerAlphaTestFunctions() {
    REGISTER(glAlphaFunc);
}

void glAlphaFunc(GLenum op, GLclampf threshold) {
    Lists::displayListManager->addCommand<glAlphaFunc>(op, threshold);
    
    if (!trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) return;
    
    FFPE::States::AlphaTest::op = op;
    FFPE::States::AlphaTest::threshold = threshold;
}
