#include "es/ffp.hpp"
#include "es/ffpe/immediate.hpp"
#include "gles/ffp/main.hpp"
#include "glm/ext/vector_float2.hpp"
#include "main.hpp"

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
