#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "glm/ext/vector_float4.hpp"
#include "main.h"

#include <GLES3/gl32.h>

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

void FFP::registerColorFunctions() {
    REGISTER(glColor4f);
}

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glColor4f>(red, green, blue, alpha);
        return;
    }

    FFPE::States::VertexData::color = glm::vec4(red, green, blue, alpha);
}

