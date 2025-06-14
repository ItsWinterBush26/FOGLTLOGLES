#include "gles/ffp/main.hpp"
#include "main.hpp"

#include <GLES3/gl32.h>

void glLightfv(GLenum light, GLenum pname, const GLfloat *params);

void glLightModelfv(GLenum pname, const GLfloat *params);

void FFP::registerLightFunctions() {
    REGISTER(glLightfv);

    REGISTER(glLightModelfv);
}

void glLightfv(GLenum light, GLenum pname, const GLfloat *params) {

}

void glLightModelfv(GLenum pname, const GLfloat *params) {

}
