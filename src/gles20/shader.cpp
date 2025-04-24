#include "shader/converter.h"
#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "utils/env.h"

#include <GLES2/gl2.h>
#include <stdexcept>
#include <string>

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
void OV_glCompileShader(GLuint shader);
void OV_glLinkProgram(GLuint program);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glShaderSource);
    REGISTEROV(glCompileShader)
    REGISTEROV(glLinkProgram);
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

        LOGI("ShaderConverter::convertAndFix()!");
        
        ShaderConverter::convertAndFix(getKindFromShader(shader), combinedSource);
        if (combinedSource.empty()) {
            LOGE("Convertion failed?");
        }
        
        const GLchar* newSource[] = { combinedSource.c_str() };
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
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetShaderInfoLog(shader, 4096, &size, bufLog);

        LOGI("Compile error for shader %u: %s", shader, bufLog);

        GLint length = 0;
        glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);
        if (length > 0) {
            GLchar* source = new GLchar[length];
            glGetShaderSource(shader, length, NULL, source);
            
            LOGI("Shader %u source:", shader);
            LOGI("%s", source);
            
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
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetProgramInfoLog(program, 4096, &size, bufLog);

        LOGI("Link error for program %u: %s", program, bufLog);
        throw std::runtime_error("Failed to link program!");
    }
}
