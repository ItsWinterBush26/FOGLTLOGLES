#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "gles30/main.h"
#include "main.h"

typedef void (*glBindFragDataLocationEXT_PTR)(GLuint program, GLuint colorNumber, const char* name);
inline glBindFragDataLocationEXT_PTR glBindFragDataLocationEXT;

void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name); 

void GLES30::registerBindFunctions() {
    REGISTER(glBindFragDataLocation);

    glBindFragDataLocationEXT = reinterpret_cast<glBindFragDataLocationEXT_PTR>(eglGetProcAddress("glBindFragDataLocationEXT"));
}

void glBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name) {
    glBindFragDataLocationEXT(program, colorNumber, name);
}