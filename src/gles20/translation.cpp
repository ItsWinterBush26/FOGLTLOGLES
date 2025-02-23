#include "gles20/translation.h"
#include "es/proxy.h"
#include "es/texture.h"
#include "es/utils.h"
#include "main.h"
#include "shaderc/shaderc.h"
#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <regex>

// TODO: texture.cpp shader.cpp separation

void glClearDepth(double d);
void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);

static GLint maxTextureSize = 0;

void GLES20::registerTranslatedFunctions() {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOGI("GL_MAX_TEXTURE_SIZE is %i", maxTextureSize);

    REGISTER(glClearDepth);
    REGISTEROV(glTexImage2D);
    REGISTEROV(glGetTexLevelParameteriv);
    REGISTEROV(glShaderSource);
}

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

GLint proxyWidth, proxyHeight, proxyInternalFormat;

void OV_glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width, GLsizei height,
    GLint border, GLenum format,
    GLenum type, const void* pixels
) {
    LOGI("glTexImage2D: internalformat=%i border=%i format=%i type=%u", internalFormat, border, format, type);
    if (isProxyTexture(target)) {
        LOGI("yes its a proxy tex");
        proxyWidth = (( width << level ) > maxTextureSize) ? 0 : width;
        proxyHeight = (( height << level ) > maxTextureSize) ? 0 : height;
        proxyInternalFormat = internalFormat;
    } else {
        selectProperTexType(internalFormat, type);
        glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
    }
}

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) {
    if (isProxyTexture(target)) {
        switch (pname) {
            case GL_TEXTURE_WIDTH:
                (*params) = nlevel(proxyWidth, level);
                return;
            case GL_TEXTURE_HEIGHT:
                (*params) = nlevel(proxyHeight, level);
                return;
            case GL_TEXTURE_INTERNAL_FORMAT:
                (*params) = proxyInternalFormat;
                return;
        }
    } else {
        glGetTexLevelParameteriv(target,level, pname, params);
    }
}

void replaceShaderVersion(std::string& shaderSource, const std::string& newVersion) {
    std::regex versionRegex(R"(#version\s+\d+(\s+\w+)?\b)");  // Ensures full match
    shaderSource = std::regex_replace(shaderSource, versionRegex, "#version " + newVersion + "\n");
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    if (!count || !sources) return;
    LOGI("glShaderSource(%d, %d, %p, %p)", shader, count, sources, length);

    std::string fullSource;
    for (GLsizei i = 0; i < count; i++) {
         if (sources[i]) {
            if (length && length[i] > 0) {
                  fullSource.append(sources[i], length[i]);
            } else {
                fullSource.append(sources[i]);
            }
        }
    }

    int glslVersion = 0;
    if (sscanf(fullSource.c_str(), "#version %i", &glslVersion) != 1) {
        throw new std::runtime_error("No #version preprocessor!");
    }

    LOGI("Shader GLSL version is %i", glslVersion);
    if (glslVersion < 330) {
        glslVersion = 330;
        replaceShaderVersion(fullSource, std::to_string(glslVersion));

        LOGI("New GLSL version is %i", glslVersion);
    }

    shaderc::Compiler spirvCompiler;
    shaderc::CompileOptions spirvOptions;
    spirvOptions.SetSourceLanguage(shaderc_source_language_glsl);
    spirvOptions.SetTargetEnvironment(shaderc_target_env_opengl, glslVersion);
    spirvOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    spirvOptions.SetAutoMapLocations(true);
    spirvOptions.SetAutoBindUniforms(true);
    spirvOptions.SetAutoSampledTextures(true);

    shaderc::SpvCompilationResult bytecode = spirvCompiler.CompileGlslToSpv(
        fullSource,
        ESUtils::getKindFromShader(shader),
        "shader",
        spirvOptions
    );

    if (bytecode.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string errorMessage = bytecode.GetErrorMessage();
        throw std::runtime_error("Shader compilation error: " + errorMessage);
    }

    spirv_cross::CompilerGLSL esslCompiler({ bytecode.cbegin(), bytecode.cend() });
    spirv_cross::CompilerGLSL::Options esslOptions;
    int esslVersion = atoi(reinterpret_cast<str>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    
    esslOptions.version = 320;
    esslOptions.es = true;
    esslOptions.force_flattened_io_blocks = true;
    esslCompiler.set_common_options(esslOptions);

    LOGI("Got ESSL version: %i", esslVersion);

    std::string esslShader = esslCompiler.compile();
    const GLchar* newSource = esslShader.c_str();

    glShaderSource(shader, 1, &newSource, nullptr);

    LOGI("Success compile!");
}
