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
GLuint OV_glCreateShader(GLenum shaderType);
void OV_glCompileShader(GLuint shader);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glCreateShader);
    REGISTEROV(glAttachShader);
    REGISTEROV(glShaderSource);
    REGISTEROV(glCompileShader);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
    REGISTEROV(glDeleteShader);
}

namespace {
    std::unordered_map<GLuint, ShaderConverter> programConverters;
    std::unordered_map<GLuint, ShaderConverter*> shaderConverters;
    std::unordered_map<GLuint, ShaderConverter> tempConverters;
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

GLuint OV_glCreateShader(GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    LOGI("OV_glCreateShader: Created shader %u of type %u", shader, shaderType);
    return shader;
}

void OV_glAttachShader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto programIt = programConverters.find(program);
        if (programIt != programConverters.end()) {
            // Check if shader has a temporary converter
            auto tempIt = tempConverters.find(shader);
            
            if (tempIt != tempConverters.end()) {
                // Transfer any source from temp converter to program converter
                auto& tempConverter = tempIt->second;
                shaderc_shader_kind kind = getKindFromShader(shader);
                std::string source = tempConverter.getShaderSource(kind);
                if (!source.empty()) {
                    programIt->second.attachSource(kind, source);
                    LOGI("OV_glAttachShader: Transferred source from temp converter to program %u for shader %u", 
                         program, shader);
                }
                tempConverters.erase(tempIt);
            }
            
            // Associate shader with program converter
            shaderConverters[shader] = &programIt->second;
            LOGI("OV_glAttachShader: Attached shader %u to program %u", shader, program);
        } else {
            LOGI("OV_glAttachShader: Program %u not found for shader %u", program, shader);
        }
    }
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    LOGI("OV_glShaderSource for shader %u", shader);

    std::string fullSource;
    combineSources(count, sources, length, fullSource);
    shaderc_shader_kind kind = getKindFromShader(shader);

    ShaderConverter* converter = nullptr;
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = shaderConverters.find(shader);
        if (it != shaderConverters.end()) {
            converter = it->second;
            LOGI("OV_glShaderSource: Using associated program converter for shader %u", shader);
        } else {
            LOGI("OV_glShaderSource: Shader %u not attached yet, creating temporary converter", shader);
            converter = &tempConverters[shader];
        }
    }

    converter->attachSource(kind, fullSource);
    std::string convertedSource = converter->getShaderSource(kind);
    const GLchar* cSource = convertedSource.c_str();

    glShaderSource(shader, 1, &cSource, nullptr);
}

void OV_glCompileShader(GLuint shader) {
    LOGI("OV_glCompileShader: Compiling shader %u", shader);
    glCompileShader(shader);
    
    // Check compilation status for debugging
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE && getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetShaderInfoLog(shader, 4096, &size, bufLog);
        LOGI("OV_glCompileShader: Compilation error for shader %u: %s", shader, bufLog);
    }
}

void OV_glLinkProgram(GLuint program) {
    LOGI("OV_glLinkProgram: Linking program %u", program);
    glLinkProgram(program);

    ShaderConverter* converter = nullptr;
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        auto it = programConverters.find(program);
        if (it == programConverters.end()) {
            throw std::runtime_error("OV_glLinkProgram: Converter not found for program");
        }
        converter = &it->second;
    }

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE && getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        GLchar bufLog[4096] = { 0 };
        GLint size = 0;
        glGetProgramInfoLog(program, 4096, &size, bufLog);
        LOGI("OV_glLinkProgram: Link error for program %u: %s", program, bufLog);
        if (getEnvironmentVar("LIBGL_VGPU_ABORT_ON_ERROR") == "1") {
            throw std::runtime_error("OV_glLinkProgram: Failed to link program!");
        }
    } else {
        LOGI("OV_glLinkProgram: Program %u linked successfully", program);
    }
}

void OV_glDeleteProgram(GLuint program) {
    LOGI("OV_glDeleteProgram: Deleting program %u", program);
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
    LOGI("OV_glDeleteShader: Deleting shader %u", shader);
    glDeleteShader(shader);
    {
        std::lock_guard<std::mutex> lock(convertersMutex);
        // Remove from shader-to-program mapping
        auto it = shaderConverters.find(shader);
        if (it != shaderConverters.end()) {
            shaderConverters.erase(it);
        }
        
        // Also check for temporary converters
        auto tempIt = tempConverters.find(shader);
        if (tempIt != tempConverters.end()) {
            tempConverters.erase(tempIt);
        } else if (it == shaderConverters.end()) {
            LOGI("OV_glDeleteShader: Shader %u not tracked", shader);
        }
    }
}
