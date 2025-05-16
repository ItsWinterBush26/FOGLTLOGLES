#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "glm/ext/vector_float4.hpp"
#include "main.h"

#include <GLES/gl.h>

void OV_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

void FFP::registerColorFunctions() {
    REGISTEROV(glColor4f);
}

void OV_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glColor4f>(red, green, blue, alpha);
        return;
    }

    if (Immediate::immediateModeState->isActive()) {
        Immediate::immediateModeState->getCurrentVertex().color = glm::vec4(red, green, blue, alpha);
        return; // idk if in spec but im not looking for allat
    }

    glColor4f(red, green, blue, alpha);
}

