#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES/gl.h>

void glTexCoord2f(GLfloat s, GLfloat t);

void FFP::registerTexCoordFunctions() {
    REGISTER(glTexCoord2f);
}

void glTexCoord2f(GLfloat s, GLfloat t) {
    Immediate::immediateModeState->setTexCoord(glm::vec4(s, t, 0, 1));
}
