#pragma once

#include "es/utils.h"
#include "gl/shader.h"
#include "postprocess.h"
#include "shaderc/shaderc.h"
#include "shaderc/status.h"
#include "spirv_glsl.hpp"
#include "utils.h"
#include "utils/compilers.h"

#include <stdexcept>
#include <string>
#include <GLES2/gl2.h>
#include <shaderc/shaderc.hpp>

class ShaderConverter {
public:
    ShaderConverter() { }
    ~ShaderConverter() {
        LOGI("Converter destroyed!");
    }

    ShaderConverter(GLuint program) : program(program), postProcessor(PostProcessor()) {
        // LOGI("New program created so new ShaderConverter created (and PostProcessor())");
    }

    void attachSource(shaderc_shader_kind kind, std::string source) {
        // LOGI("Doing magic on a %s", getKindStringFromKind(kind));
        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Recieved shader source:");
            LOGI("%s", source.c_str());
        }

        switch (kind) {
            case shaderc_vertex_shader:
                vertexSource = source;
                convertAndFix(shaderc_vertex_shader, vertexSource);
                return;
            case shaderc_fragment_shader:
                fragmentSource = source;
                convertAndFix(shaderc_fragment_shader, fragmentSource);
                return;
            default: return;
        }
    }

    void convertAndFix(shaderc_shader_kind kind, std::string& source) {
        bool isVulkanSPV = false;

        shaderc::SpvCompilationResult spirv = compileToSPV(kind, source, isVulkanSPV);
        transpileSPV2ESSL(kind, spirv, source, isVulkanSPV);
    }

    GLuint getProgram() { return program; }
    std::string getShaderSource(shaderc_shader_kind kind) {
        switch (kind) {
            case shaderc_vertex_shader:
                return vertexSource;
            case shaderc_fragment_shader:
                return fragmentSource;
            default: return "";
        }
    }

private:
    PostProcessor postProcessor;
    GLuint program;

    std::string vertexSource;
    std::string fragmentSource;


    shaderc::SpvCompilationResult compileToSPV(shaderc_shader_kind kind, std::string& source, bool& isVulkanSPV) {
        int shaderVersion = 0;
        std::string shaderProfile = "";
        getShaderVersion(source, shaderVersion, shaderProfile);
        // no need to check as shader.cpp already checks it

        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult result;

        if (shaderProfile == "es") {
            // LOGI("ESSL %i -> SPV", shaderVersion); // i assume that shaderVersion matches ESUtils::shadingVersion

            shaderc::CompileOptions options = generateESSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "esslShader", options);
            isVulkanSPV = true;
        } else {
            if (shaderVersion < 330) {
                LOGI("GLSL %i -> GLSL 330", shaderVersion);
                upgradeTo330(kind, source);
                shaderVersion = 330;
            }

            // LOGI("GLSL %i -> SPV", shaderVersion);

            shaderc::CompileOptions options = generateGLSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "glslShader", options);
        }

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            LOGE("Failed to compile shader: %s", result.GetErrorMessage().c_str());
            throw std::runtime_error("Shader compilation failed!");
        }

        return result;
    }

    void transpileSPV2ESSL(shaderc_shader_kind kind, shaderc::SpvCompilationResult& module, std::string& target, bool isVulkanSPV) {
        // LOGI("SPV -> ESSL %i", ESUtils::shadingVersion);

        spirv_cross::CompilerGLSL::Options options = generateSPV2ESSLOptions(ESUtils::shadingVersion, isVulkanSPV);

        spirv_cross::CompilerGLSL compiler({ module.cbegin(), module.cend() });
        compiler.set_common_options(options);

        postProcessor.processSPVBytecode(compiler, kind);

        target = compiler.compile();

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Transpiled GLSL -> ESSL source:");
            LOGI("%s", target.c_str());
        }
    }
};
