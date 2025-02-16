#include "shaderc/shaderc.h"
#include "spirv_glsl.hpp"
#include "wrappers/base.h"

#include "GLES2/gl2.h"
#include <shaderc/shaderc.hpp>
#include <string>

inline shaderc_shader_kind get_kind_from_shader(GLuint shader) {
    // TODO: impl
    return shaderc_glsl_compute_shader;
}

// idk
OVERRIDEV(glShaderSource, (GLuint shader, GLsizei count, const GLchar *const *source, const GLint* length)) {
    shaderc::Compiler spirvCompiler;
    shaderc::CompileOptions spirvOptions;
    spirvOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult result = spirvCompiler.CompileGlslToSpv(
        std::string(*source), get_kind_from_shader(shader),
        "kkk", spirvOptions
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