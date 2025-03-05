#include "gles20/main.h"
#include "main.h"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name); 

void GLES20::registerBindFunctions() {
    REGISTER(glBindFragDataLocation);
}

void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name) {
    glBindFragDataLocationEXT(program, colorNumber, name);
}