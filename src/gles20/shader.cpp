#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "shader/converter.h"

#include <GLES2/gl2.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <mutex>

GLuint OV_glCreateProgram();
void OV_glAttachShader(GLuint program, GLuint shader);
void OV_glLinkProgram(GLuint program);
void OV_glDeleteProgram(GLuint program);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glAttachShader);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
}

std::unordered_map<GLuint, ShaderConverter> converters;

GLuint OV_glCreateProgram() {
    GLuint program = glCreateProgram();
    converters[program] = ShaderConverter(program);
    return program;
}

void OV_glAttachShader(GLuint program, GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

    if (length > 0) {
        std::vector<GLchar> source(length);
        glGetShaderSource(shader, length, nullptr, source.data());
    
        ShaderConverter converter = converters[program];
        converter.attachSource(getKindFromShader(shader), std::string(source.data()));
        
        std::string realSource = converter.getShaderSource(getKindFromShader(shader));
        const GLchar* newSource = realSource.c_str();
        glShaderSource(shader, 1, &newSource, nullptr);
    }
    glAttachShader(program, shader);
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE && getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetProgramInfoLog(program, 4096, &size, bufLog);
        LOGI("Link error for program %u: %s", program, bufLog);
        if (getEnvironmentVar("LIBGL_VGPU_ABORT_ON_ERROR") == "1") {
            throw std::runtime_error("Failed to link program!");
        }
    }
}

void OV_glDeleteProgram(GLuint program) {
    LOGI("OV_glDeleteProgram: Deleting program %u", program);
    glDeleteProgram(program);

    ShaderConverter converter = converters[program];
    converter.finish();
    
    converters.erase(program);
}
