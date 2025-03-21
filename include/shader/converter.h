#pragma once

#include "cache.h"
#include "compilers.h"
#include "postprocess.h"
#include "es/utils.h"
#include "gl/shader.h"
#include "shaderc/shaderc.h"
#include "shaderc/status.h"
#include "spirv_glsl.hpp"
#include "utils.h"

#include <stdexcept>
#include <string>
#include <GLES2/gl2.h>
#include <shaderc/shaderc.hpp>

namespace ShaderConverter {
    inline shaderc::SpvCompilationResult compileToSPV(shaderc_shader_kind kind, std::string& source) {
        int shaderVersion = 0;
        std::string shaderProfile = ""; // useless
        getShaderVersion(source, shaderVersion, shaderProfile);

        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult result;

        if (shaderVersion < 330) {
            upgradeTo330(kind, source);
            shaderVersion = 330;
        }

        shaderc::CompileOptions options = generateGLSL2SPVOptions(shaderVersion);
        result = compiler.CompileGlslToSpv(source, kind, "glslShader", options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            LOGE("Failed to compile shader: %s", result.GetErrorMessage().c_str());
            throw std::runtime_error("Shader compilation failed!");
        }

        return result;
    }

    inline void transpileSPV2ESSL(shaderc_shader_kind kind, shaderc::SpvCompilationResult& module, std::string& target) {
        spirv_cross::CompilerGLSL::Options options = generateSPV2ESSLOptions(ESUtils::shadingVersion);

        SPVC_Exposed_CompilerGLSL compiler({ module.cbegin(), module.cend() });
        compiler.set_common_options(options);

        SPVPostprocessor::processSPVBytecode(compiler, kind);

        target = compiler.compile();

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Transpiled GLSL -> ESSL source:");
            LOGI("%s", target.c_str());
        }
    }

    inline void convertAndFix(shaderc_shader_kind kind, std::string& source) {
        size_t key = Cache::getHash(source);
        if (Cache::isShaderInCache(key)) {
            source = Cache::getCachedShaderSource(key);
            return;
        }

        shaderc::SpvCompilationResult spirv = compileToSPV(kind, source);
        transpileSPV2ESSL(kind, spirv, source);

        Cache::putShaderInCache(key, source);
    }
}; // namespace ShaderConverter