#include "shader/utils.h"
#include "gles20/main.h"
#include "main.h"
#include "shader/converter.h"
#include "utils/env.h"

#include <GLES2/gl2.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

GLuint OV_glCreateProgram();
void OV_glAttachShader(GLuint program, GLuint shader);
void OV_glCompileShader(GLuint shader);
void OV_glLinkProgram(GLuint program);
void OV_glDeleteProgram(GLuint program);
void OV_glGetShaderiv(GLuint shader, GLenum pname, GLint* params);

void GLES20::registerShaderOverrides() {
    REGISTEROV(glCreateProgram);
    REGISTEROV(glAttachShader);
    REGISTEROV(glCompileShader);
    REGISTEROV(glLinkProgram);
    REGISTEROV(glDeleteProgram);
    REGISTEROV(glGetShaderiv);
}

inline std::unordered_map<GLuint, std::shared_ptr<ShaderConverter>> converters;
inline std::unordered_map<GLuint, bool> previouslyNoop; // couldve just paired but whatever
inline std::unordered_set<std::string> requiredExtensions = {
    "GL_EXT_blend_func_extended"
};

GLuint OV_glCreateProgram() {
    GLuint program = glCreateProgram();

    // remove existing key and replace with new shaderconverter when?
    converters.insert({ program, std::make_shared<ShaderConverter>(program) });
    return program;
}

void OV_glAttachShader(GLuint program, GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

    if (length == 0) {
        throw std::runtime_error("Cannot attach shader to program with a shader that has no source!");
    }
    
    std::vector<GLchar> source(length);
    glGetShaderSource(shader, length, nullptr, source.data());
    std::string convertedSource = std::string(source.data());

    int version = 0; // useless
    std::string profile = "";
    if (!getShaderVersion(convertedSource, version, profile)) {
        throw std::runtime_error("Shader with no version preprocessor!");
    }

    if (profile != "es") {
        std::shared_ptr<ShaderConverter> converter = converters.at(program);
        converter->attachSource(getKindFromShader(shader), convertedSource);
        
        std::string realSource = converter->getShaderSource(getKindFromShader(shader));
        const GLchar* newSource = realSource.c_str();
        
        glShaderSource(shader, 1, &newSource, nullptr);
        glCompileShader(shader);
        glAttachShader(program, shader);
    } else {
        LOGI("Shader already ESSL, no need to convert!");

        std::string newSource = "";

        for (const auto& it : requiredExtensions) {
            newSource += "#extension " + it + " : enable;\n";
        }

        newSource += "\n";
        newSource += convertedSource;

        const GLchar* casted = newSource.c_str();

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Modified ESSL:");
            LOGI("%s", newSource.c_str());
        }

        glShaderSource(shader, 1, &casted, nullptr);
        glCompileShader(shader);
        glAttachShader(program, shader);
    }

    previouslyNoop.erase(shader);
}

void OV_glCompileShader(GLuint shader) {
    previouslyNoop[shader] = true;
}

void OV_glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
    switch (pname) {
        case GL_COMPILE_STATUS:
            {
                auto it = previouslyNoop.find(shader);
                if (it != previouslyNoop.end() && it->second) {
                    (*params) = GL_TRUE;
                    
                    previouslyNoop[it->first] = false;
                }
            }
            break;
        
        default:
            glGetShaderiv(shader, pname, params);

            break;
    }
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
        throw std::runtime_error("Failed to link program!");
    }

    converters.erase(program);
}

void OV_glDeleteProgram(GLuint program) {
    glDeleteProgram(program);

    if (converters.find(program) != converters.end()) {
        converters.erase(program);
    }
}
