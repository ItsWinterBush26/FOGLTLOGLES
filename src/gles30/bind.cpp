#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include "gles30/main.h"
#include "main.h"

extern "C" void glBindFragDataLocationEXT(GLuint program, GLuint colorNumber, const char* name);
void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name); 

void GLES30::registerBindFunctions() {
    REGISTER(glBindFragDataLocation);
}

void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name) {
    glBindFragDataLocationEXT(program, colorNumber, name);
}