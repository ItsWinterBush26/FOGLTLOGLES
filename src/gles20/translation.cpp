#include "gles20/translation.h"
#include "gl/glext.h"
#include "main.h"
#include "gles20/proxy.h"
#include "shaderc/env.h"
#include "shaderc/shaderc.h"
#include "spirv_glsl.hpp"
#include "utils/log.h"
#include "shaderc/shaderc.hpp"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdexcept>
#include <string>

void glClearDepth(double d);
void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);

static GLint maxTextureSize = 0;

void GLES20::registerTranslatedFunctions() {
    LOGI("Hi from translation.cpp!");
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    REGISTER(glClearDepth);
    REGISTEROV(glTexImage2D);
    REGISTEROV(glGetTexLevelParameteriv);
    REGISTEROV(glShaderSource)
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
    if (isProxyTexture(target)) {
        proxyWidth = (( width << level ) > maxTextureSize) ? 0 : width;
        proxyHeight = (( height << level ) > maxTextureSize) ? 0 : height;
        proxyInternalFormat = internalFormat;

        return;
    }

    glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
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

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* sources, const GLint* length) {
    if (!count || !sources) return;

    shaderc::Compiler spirvCompiler;
    shaderc::CompileOptions spirvOptions;
    spirvOptions.SetSourceLanguage(shaderc_source_language_glsl);
    spirvOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
    spirvOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);

    shaderc_shader_kind kind;
    GLint shaderType;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

    switch (shaderType) {
        case GL_FRAGMENT_SHADER:
            kind = shaderc_fragment_shader;
        break;
        case GL_VERTEX_SHADER:
            kind = shaderc_vertex_shader;
        break;
        default:
            LOGI("%u", shader);
            throw std::runtime_error("Received an unsupported shader type!");
        /* GL_COMPUTE_SHADER is in GLES3 */
    }

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

    shaderc::SpvCompilationResult bytecode = spirvCompiler.CompileGlslToSpv(
        fullSource, kind, 
        "shader", spirvOptions
    );

    spirv_cross::CompilerGLSL esslCompiler({ bytecode.cbegin(), bytecode.cend() });
    spirv_cross::CompilerGLSL::Options esslOptions;
    esslOptions.version = 200;
    esslOptions.es = true;
    esslOptions.force_flattened_io_blocks = true;
    esslCompiler.set_common_options(esslOptions);

    std::string esslShader = esslCompiler.compile();
    const GLchar* newSource = esslShader.c_str();

    glShaderSource(shader, 1, &newSource, nullptr);
}