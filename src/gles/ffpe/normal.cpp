#include "es/ffpe/lists.hpp"
#include "es/ffpe/states.hpp"
#include "es/ffpe/immediate.hpp"
#include "gles/ffp/main.hpp"
#include "glm/ext/vector_float3.hpp"
#include "main.hpp"

#include <GLES3/gl32.h>

void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);

void FFP::registerNormalFunctions() {
    REGISTER(glNormal3f);
}

void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) {
    if (!FFPE::Rendering::ImmediateMode::isActive()) {
        FFPE::List::addCommand<glNormal3f>(nx, ny, nz);
        return;
    }

    FFPE::States::VertexData::set(
        glm::vec3(nx, ny, nz),
        FFPE::States::VertexData::normal
    );
}

