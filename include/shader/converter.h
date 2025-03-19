#pragma once

#include "es/utils.h"
#include "gl/shader.h"
#include "postprocess.h"
#include "shaderc/shaderc.h"
#include "shaderc/status.h"
#include "spirv_glsl.hpp"
#include "utils.h"
#include "shader/compilers.h"

#include <stdexcept>
#include <string>
#include <GLES2/gl2.h>
#include <shaderc/shaderc.hpp>

namespace ShaderConverter {
    inline shaderc::SpvCompilationResult compileToSPV(shaderc_shader_kind kind, std::string& source, bool& isVulkanSPV) {
        int shaderVersion = 0;
        std::string shaderProfile = "";
        getShaderVersion(source, shaderVersion, shaderProfile);

        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult result;

        if (shaderProfile == "es") {
            shaderc::CompileOptions options = generateESSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "esslShader", options);
            isVulkanSPV = true;
        } else {
            if (shaderVersion < 330) {
                LOGI("GLSL %i -> GLSL 330", shaderVersion);
                upgradeTo330(kind, source);
                shaderVersion = 330;
            }

            shaderc::CompileOptions options = generateGLSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "glslShader", options);
        }

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            LOGE("Failed to compile shader: %s", result.GetErrorMessage().c_str());
            throw std::runtime_error("Shader compilation failed!");
        }

        return result;
    }

    inline void transpileSPV2ESSL(shaderc_shader_kind kind, shaderc::SpvCompilationResult& module, std::string& target, bool isVulkanSPV) {
        spirv_cross::CompilerGLSL::Options options = generateSPV2ESSLOptions(ESUtils::shadingVersion, isVulkanSPV);

        spirv_cross::CompilerGLSL compiler({ module.cbegin(), module.cend() });
        compiler.set_common_options(options);

        SPVPostprocessor::processSPVBytecode(compiler, kind);
        
        target = compiler.compile();

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Transpiled GLSL -> ESSL source:");
            LOGI("%s", target.c_str());
        }
    }

    inline void convertAndFix(shaderc_shader_kind kind, std::string& source) {
        bool isVulkanSPV = false;

        shaderc::SpvCompilationResult spirv = compileToSPV(kind, source, isVulkanSPV);
        transpileSPV2ESSL(kind, spirv, source, isVulkanSPV);
    }
}; // namespace ShaderConverter