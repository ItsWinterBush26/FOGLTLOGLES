#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "shader/converter.h"

#include <GLES2/gl2.h>
#include <string>

GLuint OV_glCreateProgram();
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
void OV_glLinkProgram(GLuint program);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glShaderSource);
    REGISTEROV(glLinkProgram);
}

ShaderConverter converter;

GLuint OV_glCreateProgram() {
    converter = ShaderConverter(glCreateProgram());
    return converter.getProgram();
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    std::string fullSource;
    combineSources(count, sources, length, fullSource);
    
    converter.attachSource(getKindFromShader(shader), fullSource);
    auto source = converter.getShaderSource(getKindFromShader(shader));
    const GLchar* cSource = source.c_str();

    glShaderSource(shader, 1, &cSource, nullptr);
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    converter.finish();
    converter = ShaderConverter();
}