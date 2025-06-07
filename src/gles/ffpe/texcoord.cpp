#include "es/ffp.h"
#include "es/ffpe/immediate.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glTexCoord2f(GLfloat s, GLfloat t);

void FFP::registerTexCoordFunctions() {
    REGISTER(glTexCoord2f);
}

void glTexCoord2f(GLfloat s, GLfloat t) {
    if (!FFPE::Rendering::ImmediateMode::isActive()) {
        Lists::displayListManager->addCommand<glTexCoord2f>(s, t);
    }

    FFPE::States::VertexData::set(
        glm::vec2(s, t),
        &FFPE::States::VertexData::texCoord
    );
}
