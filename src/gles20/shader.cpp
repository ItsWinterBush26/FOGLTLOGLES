#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "shader/converter.h"

#include <GLES2/gl2.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define CHECKS

GLuint OV_glCreateProgram();
void OV_glShaderSource(GLuint shader, GLint count, const GLchar* const* sources, const GLint* length);
void OV_glAttachShader(GLuint program, GLuint shader);
void OV_glCompileShader(GLuint shader);
void OV_glLinkProgram(GLuint program);
void OV_glDeleteProgram(GLuint program);
void OV_glGetShaderiv(GLuint shader, GLenum pname, GLint* params);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    // disable for now
    // REGISTEROV(glShaderSource);
    REGISTEROV(glAttachShader);
    REGISTEROV(glCompileShader);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
    REGISTEROV(glGetShaderiv);
}

inline std::unordered_map<GLuint, std::shared_ptr<ShaderConverter>> converters;
inline bool wasNoop;

GLuint OV_glCreateProgram() {
    GLuint program = glCreateProgram();
    LOGI("OV_glCreateProgram: New program %u", program);
    // remove existing key and replace with new shaderconverter when?
    converters.insert({ program, std::make_shared<ShaderConverter>(program) });
    return program;
}

void OV_glAttachShader(GLuint program, GLuint shader) {
    LOGI("OV_glAttachShader: Attaching %u (shader) to %u (program)", shader, program);
    
    GLint length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

    if (length != 0) {
        std::vector<GLchar> source(length);
        glGetShaderSource(shader, length, nullptr, source.data());
    
        std::shared_ptr<ShaderConverter> converter = converters.at(program);
        converter->attachSource(getKindFromShader(shader), std::string(source.data()));
        
        std::string realSource = converter->getShaderSource(getKindFromShader(shader));
        const GLchar* newSource = realSource.c_str();
        glShaderSource(shader, 1, &newSource, nullptr);
        glCompileShader(shader);
        glAttachShader(program, shader);

        return;
    }

    throw std::runtime_error("Cannot attach shader to program with a shader that has no source!");
}

void OV_glCompileShader(GLuint shader) {
    LOGI("OV_glCompileShader: Shader %u", shader);
    LOGI("NOOP :>");
    wasNoop = true;
}

void OV_glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
    LOGI("OV_getShaderiv: Shader %u", shader);
    switch (pname) {
        case GL_COMPILE_STATUS:
            if (wasNoop) {
                LOGI("It's gaslight time! :devil:");
                (*params) = GL_TRUE;

                wasNoop = false;
                return;
            }
        default: break;
    }

    glGetShaderiv(shader, pname, params);
}

// Lowkey just remove this
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* sources, const GLint* length) {
    LOGI("OV_glShaderSource: Shader %u", shader);
    LOGI("Passtrough");
    glShaderSource(shader, count, sources, nullptr);
}

void OV_glLinkProgram(GLuint program) {
    LOGI("OV_glLinkProgram: Linking program %u", program);
    glLinkProgram(program);

    #ifdef CHECKS
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
    #endif

    LOGI("Linked! Removing from converter map.");
    std::shared_ptr<ShaderConverter> converter = converters.at(program);
    converter->finish();
    converters.erase(program);
}

void OV_glDeleteProgram(GLuint program) {
    LOGI("OV_glDeleteProgram: Deleting program %u", program);
    glDeleteProgram(program);

    if (converters.find(program) != converters.end()) {
        std::shared_ptr<ShaderConverter> converter = converters.at(program);
        converter->finish();
        converters.erase(program);
    }
}
