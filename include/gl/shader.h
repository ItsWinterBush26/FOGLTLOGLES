#pragma once

#include "es/utils.h"
#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "utils/env.h"
#include "utils/log.h"

#include <stdexcept>

inline void autoAssignLocations(spirv_cross::CompilerGLSL& compiler) {
    auto resources = compiler.get_shader_resources();

    // Assign locations for vertex shader inputs
    int location = 0;
    for (const auto& input : resources.stage_inputs) {
        compiler.set_decoration(input.id, spv::DecorationLocation, location++);
    }

    // Assign locations for fragment shader inputs (varyings)
    location = 0;
    for (const auto& output : resources.stage_outputs) {
        compiler.set_decoration(output.id, spv::DecorationLocation, location++);
    }
}

inline void upgradeTo330(shaderc_shader_kind kind, std::string& src) {
    LOGI("Upgrading shader to GLSL 330");

    shaderc::CompileOptions options;
    options.SetGenerateDebugInfo();
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetTargetEnvironment(shaderc_target_env_opengl, 330);
    options.SetForcedVersionProfile(330, shaderc_profile_core);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    options.SetAutoMapLocations(true);
    // options.SetAutoBindUniforms(true);
    options.SetAutoSampledTextures(true);

    shaderc::Compiler spirvCompiler;
    shaderc::SpvCompilationResult module = spirvCompiler.CompileGlslToSpv(
        src, kind, 
        "shader_upgto330", options
    );

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error("Shader upgrade error: " + module.GetErrorMessage());
    }

    spirv_cross::CompilerGLSL glslCompiler({ module.cbegin(), module.cend() });
    spirv_cross::CompilerGLSL::Options glslOptions;
    glslOptions.version = ESUtils::shadingVersion;
    glslOptions.es = true;
    glslOptions.vulkan_semantics = false;
    glslOptions.enable_420pack_extension = false;
    glslOptions.force_flattened_io_blocks = true;
    glslOptions.enable_storage_image_qualifier_deduction = false;

    glslCompiler.set_common_options(glslOptions);
    glslCompiler.add_header_line("precision mediump float;");
    glslCompiler.add_header_line("precision highp int;");

    autoAssignLocations(glslCompiler);
    src = glslCompiler.compile();

    if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
        LOGI("Upgrade shader source:");
        LOGI("%s", src.c_str());
    }

    LOGI("Upgrade successful!");
}
