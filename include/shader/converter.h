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
        int tmpVersion = 0;
        std::string profile = "";

        if (!getShaderVersion(source, tmpVersion, profile)) {
            throw new std::runtime_error("Shader with no version preprocessor");
        }

        if (profile != "es") {
            LOGI("GLSL -> SPV");
            shaderc::SpvCompilationResult spirv = compileGLSl2SPV(kind, source);
            LOGI("SPV -> ESSL");
            transpileSPV2ESSL(kind, spirv, source);
        } else LOGI("Shader already ESSL, no conversion needed");
    }

    void finish() {
        LOGI("Linked!");
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

    int ensure330(shaderc_shader_kind kind, std::string& source, int& version) {
        int detectedVersion = 0;
        std::string detectedProfile = "";
        if (!getShaderVersion(source, detectedVersion, detectedProfile)) {
            throw std::runtime_error("Shader with no version preprocessor!");
        }

        if (detectedVersion < 330 && detectedProfile != "es") {
            LOGI("GLSL %i -> GLSL 330", detectedVersion);

            upgradeTo330(kind, source);
            detectedVersion = 330;
        }

        version = detectedVersion;
    }

    shaderc::SpvCompilationResult compileGLSl2SPV(shaderc_shader_kind kind, std::string& source) {
        if (source.empty()) return shaderc::SpvCompilationResult();

        int glslVersion = 0;
        ensure330(kind, source, glslVersion);

        shaderc::Compiler compiler;
        shaderc::CompileOptions options = generateGLSL2SPVOptions(glslVersion);
        
        return compiler.CompileGlslToSpv(source, kind, "shader", options);
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
