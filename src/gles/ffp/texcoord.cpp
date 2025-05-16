#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES/gl.h>

void glTexCoord2f(GLfloat s, GLfloat t);

void FFP::registerTexCoordFunctions() {
    REGISTER(glTexCoord2f);
}

void glTexCoord2f(GLfloat s, GLfloat t) {
    if (!Immediate::immediateModeState->isActive()) return;
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glTexCoord2f>(s, t);
        return;
    }

    Immediate::immediateModeState->setTexCoord(glm::vec4(s, t, 0, 1));
}
