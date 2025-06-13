#include "es/ffpe/lists.hpp"
#include "es/ffpe/states.hpp"
#include "gles/ffp/enums.hpp"
#include "gles/ffp/main.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "main.hpp"

#include <GLES3/gl32.h>

void glFogi(GLenum pname, GLint param);

void glFogf(GLenum pname, GLfloat param);
void glFogfv(GLenum pname, const GLfloat* params);

void FFP::registerFogFunctions() {
    REGISTER(glFogi);

    REGISTER(glFogf);
    REGISTER(glFogfv);
}

void glFogi(GLenum pname, GLint param) {
    if (FFPE::List::addCommand<glFogi>(pname, param)) return;

    switch (pname) {
        case GL_FOG_MODE:
            FFPE::States::Fog::fogMode = param;
        break;
    }
}

void glFogf(GLenum pname, GLfloat param) {
    if (FFPE::List::addCommand<glFogf>(pname, param)) return;

    switch (pname) {
        case GL_FOG_START:
            FFPE::States::Fog::Linear::fogStart = param;
        break;

        case GL_FOG_END:
            FFPE::States::Fog::Linear::fogEnd = param;
        break;

        case GL_FOG_DENSITY:
            FFPE::States::Fog::Exp::fogDensity = param;
        break;
    }
}


void glFogfv(GLenum pname, const GLfloat* param) {
    if (FFPE::List::addCommand<glFogfv>(pname, param)) return;

    switch (pname) {
        case GL_FOG_COLOR:
            FFPE::States::Fog::fogColor = glm::make_vec4(param);
        break;
    }
}
