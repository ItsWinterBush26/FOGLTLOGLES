#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "shader/converter.h"

#include <GLES2/gl2.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

GLuint OV_glCreateProgram();
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
void OV_glLinkProgram(GLuint program);
void OV_glDeleteProgram(GLuint program);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glShaderSource);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
}

std::unordered_map<GLuint, ShaderConverter> converters;
ShaderConverter currentConverter;

GLuint OV_glCreateProgram() {
    currentConverter = ShaderConverter(glCreateProgram());

    converters[currentConverter.getProgram()] = currentConverter;
    return currentConverter.getProgram();
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    LOGI("glShaderSource %s", getKindStringFromKind(getKindFromShader(shader)));

    std::string fullSource;
    combineSources(count, sources, length, fullSource);
    
    currentConverter.attachSource(getKindFromShader(shader), fullSource);
    auto source = currentConverter.getShaderSource(getKindFromShader(shader));
    const GLchar* cSource = source.c_str();

    glShaderSource(shader, 1, &cSource, nullptr);
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    ShaderConverter converter = converters[program];

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1" && success != GL_TRUE) {
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;

        glGetProgramInfoLog(program, 4096, &size, bufLog);

        LOGI("Link error: %s", bufLog);

        throw std::runtime_error("Failed to link program!");
    }

    // converter.finish();
}

void OV_glDeleteProgram(GLuint program) {
    glDeleteProgram(program);

    ShaderConverter converter = converters[program];
    converter.finish();
    converters.erase(program);
}