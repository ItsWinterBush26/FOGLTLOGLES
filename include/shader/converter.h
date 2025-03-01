#pragma once

#include "es/utils.h"
#include "gl/shader.h"
#include "postprocess.h"
#include "shaderc/shaderc.h"
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
    ShaderConverter(GLuint program) : program(program), postProcessor(PostProcessor()) {
        LOGI("New program created so new ShaderConverter created (and PostProcessor())");
    }

    void attachSource(shaderc_shader_kind kind, std::string source) {
        LOGI("Doing magic on a %s", getKindStringFromKind(kind));
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
        LOGI("GLSL/ESSL -> SPV");
        shaderc::SpvCompilationResult spirv = compileToSPV(kind, source);
        LOGI("SPV -> ESSL");
        transpileSPV2ESSL(kind, spirv, source);
    }

    void finish() {
        LOGI("Program destroyed!");
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

    shaderc::SpvCompilationResult compileToSPV(shaderc_shader_kind kind, std::string& source) {
        if (source.empty()) return shaderc::SpvCompilationResult();

        int shaderVersion = 0;
        std::string shaderProfile = "";
        if (!getShaderVersion(source, shaderVersion, shaderProfile)) {
            throw std::runtime_error("Shader with no version preprocessor!");
        }

        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult result;

        if (shaderProfile == "es") {
            shaderc::CompileOptions options = generateESSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "esslShader", options);
        } else {
            if (shaderVersion < 330) {
                upgradeTo330(kind, source);
                shaderVersion = 330;
            }

            shaderc::CompileOptions options = generateGLSL2SPVOptions(shaderVersion);
            result = compiler.CompileGlslToSpv(source, kind, "glslShader", options);
        }

        return result;
    }

    void transpileSPV2ESSL(shaderc_shader_kind kind, shaderc::SpvCompilationResult& module, std::string& target) {
        spirv_cross::CompilerGLSL::Options options = generateSPV2ESSLOptions(ESUtils::shadingVersion);

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
