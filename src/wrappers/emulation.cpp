#include "spirv_glsl.hpp"
#include "wrappers/base.h"

#include "GLES2/gl2.h"
#include <shaderc/shaderc.hpp>
#include <string>

inline shaderc_shader_kind get_kind_from_shader(GLuint shader) {
    switch (shader) {
        case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
        /* case GL_COMPUTE_SHADER:
            return std::string(glGetString(GL_VERSION)) == std::string("3.2") ? shaderc_glsl_geometry_shader : shaderc_glsl_miss_shader; */
        default: return shaderc_glsl_fragment_shader;
    }
}

// idk
OVERRIDEV(glShaderSource, (GLuint shader, GLsizei count, const GLchar *const *source, const GLint* length)) {
    shaderc::Compiler spirvCompiler;
    shaderc::CompileOptions spirvOptions;
    spirvOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc_shader_kind shaderKind = get_kind_from_shader(shader);
    shaderc::SpvCompilationResult result = spirvCompiler.CompileGlslToSpv(
        std::string(*source), shaderKind,
        "0ejeif9v", spirvOptions
    );

    std::vector<uint32_t> spirvShader = { result.cbegin(), result.cend() };

    spirv_cross::CompilerGLSL glslCompiler(spirvShader);
    spirv_cross::CompilerGLSL::Options glslOptions;
    glslOptions.version = 320; // TODO: Match with device max GLES version
    glslOptions.es = true;
    glslCompiler.set_common_options(glslOptions);

    std::string esslShader = glslCompiler.compile();

    const GLchar* esslShaderCStr = esslShader.c_str();
    const GLint esslShaderLength = static_cast<GLint>(esslShader.size());

    original_glShaderSource(
        shader, count,
        &esslShaderCStr,
        &esslShaderLength
    );
}