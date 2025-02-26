#pragma once

#include "es/utils.h"
#include "gl/shader.h"
#include "shaderc/env.h"
#include "shaderc/shaderc.h"
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/compilers.h"

#include <stdexcept>
#include <string>
#include <GLES2/gl2.h>
#include <shaderc/shaderc.hpp>

class ShaderConverter {
public:
    ShaderConverter() { }
    ShaderConverter(GLuint program) : program(program) {}

    void attachSource(shaderc_shader_kind kind, std::string source) {
        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Recieved shader source:");
            LOGI("%s", source.c_str());
        }

        switch (kind) {
            case shaderc_vertex_shader:
                vertexSource = source;
                convertAndFix(kind, vertexSource);
                return;
            case shaderc_fragment_shader:
                fragmentSource = source;
                convertAndFix(kind, fragmentSource);
                return;
            default: return;
        }
    }

    void convertAndFix(shaderc_shader_kind kind, std::string& source) {
        LOGI("GLSL -> SPV");
        shaderc::SpvCompilationResult spirv = compileGLSl2SPV(shaderc_vertex_shader, source);
        LOGI("SPV -> ESSL");
        transpileSPV2ESSL(kind, spirv, source);
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
    GLuint program;

    std::string vertexSource;
    std::string fragmentSource;

    int ensure330(shaderc_shader_kind kind, std::string& source) {
        int detectedVersion = 0;
        if (sscanf(source.c_str(), "#version %i", &detectedVersion) != 1) {
            throw std::runtime_error("Source with no version preprocessor!");
        }

        if (detectedVersion < 330) {
            detectedVersion = 330;

            LOGI("GLSL <330 -> GLSL 330");

            upgradeTo330(kind, source);
        }

        return detectedVersion;
    }

    shaderc::SpvCompilationResult compileGLSl2SPV(shaderc_shader_kind kind, std::string& source) {
        if (source.empty()) return shaderc::SpvCompilationResult();

        int glslVersion = ensure330(kind, source);

        shaderc::Compiler compiler;
        shaderc::CompileOptions options = generateGLSL2SPVOptions(glslVersion);

        return compiler.CompileGlslToSpv(source, kind, "shader", options);
    }

    static std::unordered_map<std::string, uint32_t> g_uniformLocationMap;
    static uint32_t g_nextUniformLocation = 0;

    static std::unordered_map<std::string, uint32_t> g_varyingLocationMap;
    static uint32_t g_nextVaryingLocation = 0;

    void transpileSPV2ESSL(shaderc_shader_kind kind, shaderc::SpvCompilationResult& module, std::string& target) {
        spirv_cross::CompilerGLSL::Options options = generateSPV2ESSLOptions(ESUtils::shadingVersion);

        spirv_cross::CompilerGLSL compiler({ module.cbegin(), module.cend() });
        compiler.set_common_options(options);
        compiler.add_header_line("precision mediump float;");
        compiler.add_header_line("precision highp int;");


        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        for (auto &img : resources.separate_images) {
    std::string name = img.name;
    if (g_uniformLocationMap.find(name) == g_uniformLocationMap.end()) {
        g_uniformLocationMap[name] = g_nextUniformLocation++;
    }
    compiler.set_decoration(img.id, spv::DecorationLocation, g_uniformLocationMap[name]);
}

for (auto &ubo : resources.uniform_buffers) {
    auto type = compiler.get_type(ubo.type_id);
    for (uint32_t i = 0; i < type.member_types.size(); ++i) {
        std::string memberName = compiler.get_member_name(ubo.id, i);
        if (memberName.empty()) {
            memberName = "ub_member_" + std::to_string(i);
        }
        if (g_uniformLocationMap.find(memberName) == g_uniformLocationMap.end()) {
            g_uniformLocationMap[memberName] = g_nextUniformLocation++;
        }
        compiler.set_decoration(ubo.id, i, spv::DecorationLocation, g_uniformLocationMap[memberName]);
    }
}

switch (kind) {
    case shaderc_vertex_shader:
        for (auto &var : resources.stage_outputs) {
            std::string name = var.name;
            if (g_varyingLocationMap.find(name) == g_varyingLocationMap.end()) {
                g_varyingLocationMap[name] = g_nextVaryingLocation++;
            }
            compiler.set_decoration(var.id, spv::DecorationLocation, g_varyingLocationMap[name]);
        }
        break;

    case shaderc_fragment_shader:
        for (auto &var : resources.stage_inputs) {
            std::string name = var.name;
            if (g_varyingLocationMap.find(name) == g_varyingLocationMap.end()) {
                g_varyingLocationMap[name] = g_nextVaryingLocation++;
            }
            compiler.set_decoration(var.id, spv::DecorationLocation, g_varyingLocationMap[name]);
        }
        break;
}

        target = compiler.compile();

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Transpiled GLSL -> ESSL source:");
            LOGI("%s", target.c_str());
        }
    }
};
