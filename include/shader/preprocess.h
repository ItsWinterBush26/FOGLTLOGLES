#pragma once

#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <cstdint>
#include <shaderc/shaderc.hpp>
#include <string>
#include <unordered_map>

class Preprocessor {
private:
std::unordered_map<std::string, int> uniformLocationMap;
uint32_t nextAvailableUniformLocation = 0;

std::unordered_map<std::string, int> varyingLocationMap;
uint32_t nextAvailableVaryingLocation = 0;

void processSamplerUniforms(
    spirv_cross::CompilerGLSL& compiler,
    spirv_cross::SmallVector<spirv_cross::Resource> separateImages
) {
    for (auto& img : separateImages) {
        std::string name = img.name;

        if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
            uniformLocationMap[name] = nextAvailableUniformLocation++;
        }

        compiler.set_decoration(img.id, spv::DecorationLocation, uniformLocationMap[name]);
    }
}

void processUniformBuffers(
    spirv_cross::CompilerGLSL& compiler,
    spirv_cross::SmallVector<spirv_cross::Resource> uniformBuffers
) {
    for (auto &ubo : uniformBuffers) {
        auto type = compiler.get_type(ubo.type_id);
        for (uint32_t i = 0; i < type.member_types.size(); ++i) {
            std::string memberName = compiler.get_member_name(ubo.id, i);
            if (memberName.empty()) {
                memberName = "ub_member_" + std::to_string(i);
            }
            if (uniformLocationMap.find(memberName) == uniformLocationMap.end()) {
                uniformLocationMap[memberName] = nextAvailableUniformLocation++;
            }
            compiler.set_member_decoration(ubo.id, i, spv::DecorationLocation, uniformLocationMap[memberName]);
        }
    } 
}

void processVaryings(
    spirv_cross::CompilerGLSL &compiler,
    const spirv_cross::ShaderResources &resources,
    shaderc_shader_kind kind
) {
    switch (kind) {
        case shaderc_vertex_shader:
            for (auto &var : resources.stage_outputs) {
                std::string name = var.name;
                if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                        varyingLocationMap[name] = nextAvailableVaryingLocation++;
                }
                compiler.set_decoration(var.id, spv::DecorationLocation, varyingLocationMap[name]);
            }
        break;

        case shaderc_fragment_shader:
            for (auto &var : resources.stage_inputs) {
                std::string name = var.name;
                if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                        varyingLocationMap[name] = nextAvailableVaryingLocation++;
                }
                compiler.set_decoration(var.id, spv::DecorationLocation, varyingLocationMap[name]);
            }
        break;
        default: break;
    }
}

public:
void processSPVBytecode(spirv_cross::CompilerGLSL& compiler, shaderc_shader_kind kind) {
    LOGI("Processing SPIRV bytecode");

    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    processSamplerUniforms(compiler, resources.separate_images);
    processUniformBuffers(compiler, resources.uniform_buffers);
    processVaryings(compiler, resources, kind);

    LOGI("Success! NAUL=%u NAVL=%u", nextAvailableUniformLocation, nextAvailableVaryingLocation);
}

};