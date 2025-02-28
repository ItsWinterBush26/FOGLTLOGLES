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
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length);
void OV_glLinkProgram(GLuint program);
void OV_glDeleteProgram(GLuint program);
void OV_glDeleteShader(GLuint shader);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glAttachShader);
    REGISTEROV(glShaderSource);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
    REGISTEROV(glDeleteShader);
}

namespace {
    std::unordered_map<GLuint, ShaderConverter> programConverters;
    std::unordered_map<GLuint, ShaderConverter*> shaderConverters;
    std::mutex convertersMutex;
}

GLuint OV_glCreateProgram() {
    GLuint program = glCreateProgram();
    ShaderConverter converter(program);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        programConverters[program] = converter;
    }
    return program;
}

void OV_glAttachShader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = programConverters.find(program);
        if (it != programConverters.end()) {
            shaderConverters[shader] = &it->second;
        } else {
            LOGI("OV_glAttachShader: Program %u not found for shader %u", program, shader);
        }
    }
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    LOGI("OV_glShaderSource for shader %u", shader);

    std::string fullSource;
    combineSources(count, sources, length, fullSource);

    ShaderConverter* converter = nullptr;
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = shaderConverters.find(shader);
        if (it != shaderConverters.end()) {
            converter = it->second;
        } else {
            LOGI("OV_glShaderSource: Shader %u not attached yet, creating temporary converter", shader);
            static std::unordered_map<GLuint, ShaderConverter> tempConverters;
            converter = &tempConverters[shader];
        }
    }

    converter->attachSource(getKindFromShader(shader), fullSource);
    std::string convertedSource = converter->getShaderSource(getKindFromShader(shader));
    const GLchar* cSource = convertedSource.c_str();

    glShaderSource(shader, 1, &cSource, nullptr);
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    ShaderConverter converter(0);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = programConverters.find(program);
        if (it == programConverters.end()) {
            throw std::runtime_error("OV_glLinkProgram: Converter not found for program");
        }
        converter = it->second;
    }

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1" && success != GL_TRUE) {
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetProgramInfoLog(program, 4096, &size, bufLog);
        LOGI("OV_glLinkProgram Link error: %s", bufLog);
        throw std::runtime_error("OV_glLinkProgram: Failed to link program!");
    }
}

void OV_glDeleteProgram(GLuint program) {
    glDeleteProgram(program);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = programConverters.find(program);
        if (it != programConverters.end()) {
            it->second.finish();
            programConverters.erase(it);
        } else {
            LOGI("OV_glDeleteProgram: Program %u not tracked", program);
        }
    }
}

void OV_glDeleteShader(GLuint shader) {
    glDeleteShader(shader);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = shaderConverters.find(shader);
        if (it != shaderConverters.end()) {
            shaderConverters.erase(it);
        } else {
            LOGI("OV_glDeleteShader: Shader %u not tracked", shader);
        }
    }
}
