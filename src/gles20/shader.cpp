#include "es/state_tracking.h"
#include "shader/converter.h"
#include "shader/utils.h"
#include "gles20/main.h"
#include "gles20/shader_overrides.h"
#include "main.h"
#include "utils/env.h"

#include <GLES2/gl2.h>
#include <stdexcept>
#include <string>

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glShaderSource);
    REGISTEROV(glCompileShader)
    REGISTEROV(glLinkProgram);
    REGISTEROV(glUseProgram);
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) {
    std::string combinedSource;
    combineSources(count, string, length, combinedSource);

    if (combinedSource.empty()) {
        LOGW("glShaderSource was called without a shader source? Skipping...");
        return;
    }
    
    int version = 0; // useless
    std::string profile = "";
    if (!getShaderVersion(combinedSource, version, profile)) {
        throw std::runtime_error("Shader with no version preprocessor!");
    }

    if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        LOGI("Received GLSL source:");
        LOGI("%s", combinedSource.data());
    }

    if (profile != "es") {
        if (profile == "core" || profile == "compatibility") LOGI("Shader is on '%s' profile! Let's see how this goes...", profile.c_str());
        
        ShaderConverter::convertAndFix(getKindFromShader(shader), combinedSource);

        const GLchar* newSource = combinedSource.c_str();
        glShaderSource(shader, 1, &newSource, nullptr);
    } else {
        LOGI("Shader already ESSL, no need to convert");
        glShaderSource(shader, 1, string, nullptr);
    }
}

void OV_glCompileShader(GLuint shader) {
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        ShaderConverter::invalidateCurrentShader();

        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar* bufLog = new GLchar[logLength];
            glGetShaderInfoLog(shader, logLength, nullptr, bufLog);

            LOGE("Compile error for shader %u:", shader);
            LOGE("%s", bufLog);

            delete[] bufLog;
        }

        GLint sourceLength = 0;
        glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceLength);
        if (sourceLength > 0) {
            GLchar* source = new GLchar[sourceLength];
            glGetShaderSource(shader, sourceLength, nullptr, source);
            
            LOGE("Shader %u source:", shader);
            LOGE("%s", source);
            
            delete[] source;
        }

        throw std::runtime_error("Failed to compile shader!");
    }
}

void OV_glLinkProgram(GLuint program) {
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            GLchar* bufLog = new GLchar[logLength];
            glGetProgramInfoLog(program, logLength, nullptr, bufLog);

            LOGE("Link error for program %u:", program);
            LOGE("%s", bufLog);

            delete[] bufLog;
        }

        throw std::runtime_error("Failed to link program!");
    }
}

void OV_glUseProgram(GLuint program) {
    glUseProgram(program);

    trackedStates->lastUsedProgram = program;
}
