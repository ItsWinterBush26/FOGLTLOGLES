#pragma once

#include "shaderc/shaderc.hpp"
#include "utils/log.h"

#include <stdexcept>

inline void upgradeTo330(shaderc_shader_kind kind, std::string& src) {
    LOGI("Upgrading shader to GLSL 330");

    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    options.SetForcedVersionProfile(330, shaderc_profile_core);

    options.SetAutoMapLocations(true);
    options.SetAutoBindUniforms(true);
    options.SetAutoSampledTextures(true);

    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
        src, kind, 
        "shader_upgto330", options
    );

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error("Shader upgrade error: " + module.GetErrorMessage());
    }

    src = std::string(module.cbegin(), module.cend());
    LOGI("Upgrade successful!");
}