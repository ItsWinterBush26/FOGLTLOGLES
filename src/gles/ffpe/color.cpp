#include "es/ffpe/lists.hpp"
#include "es/ffpe/states.hpp"
#include "es/ffpe/immediate.hpp"
#include "gles/ffp/main.hpp"
#include "glm/ext/vector_float4.hpp"
#include "main.hpp"

#include <GLES3/gl32.h>


void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

void FFP::registerColorFunctions() {
    REGISTER(glColor4f);
}

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    if (!FFPE::Rendering::ImmediateMode::isActive()) {
        FFPE::List::addCommand<glColor4f>(red, green, blue, alpha);
        return;
    }

    FFPE::States::VertexData::set(
        glm::vec4(red, green, blue, alpha),
        FFPE::States::VertexData::color
    );
}

