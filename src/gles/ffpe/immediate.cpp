#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"
#include "utils/pointers.h"

#include <GLES3/gl32.h>

void glBegin(GLenum mode);
void glEnd();

void FFP::registerImmediateFunctions() {
    REGISTER(glBegin);
    REGISTER(glEnd);

    Immediate::immediateModeState = MakeAggregateShared<Immediate::ImmediateModeState>();
}

void glBegin(GLenum mode) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glBegin>(mode);
        return;
    }
    
    switch (mode) {
        case GL_POINTS:
        case GL_LINES:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_TRIANGLES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case 0x7: // GL_QUADS
        case 0x8: // GL_QUAD_STRIP
        case 0x9: // GL_POLYGON
            break;
        default:
            return;
    }

    Immediate::immediateModeState->begin(mode);
}

void glEnd() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glEnd>();
        return;
    }

    Immediate::immediateModeState->end();
}
