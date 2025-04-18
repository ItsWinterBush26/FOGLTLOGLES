#pragma once

#include "shaderc/shaderc.h"
#include "spirv.hpp"
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <shaderc/shaderc.hpp>
#include <string>
#include <unordered_map>

class SPVCExposed_CompilerGLSL : public spirv_cross::CompilerGLSL {
    public:
        using spirv_cross::CompilerGLSL::CompilerGLSL;
        using spirv_cross::CompilerGLSL::get;
};

namespace ShaderConverter::SPVCPostprocessor {
    enum {
        rLocation = 1 << 1,
        rBinding = 1 << 2,
        rDescSet = 1 << 3,
        rDefault = rLocation | rBinding | rDescSet
    };

    inline void removeLocationBindingAndDescriptorSets(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources,
        int flags = rDefault
    ) {
        for (const auto& resource : resources) {
            if (flags & rLocation) compiler.unset_decoration(resource.id, spv::DecorationLocation);
            if (flags & rBinding) compiler.unset_decoration(resource.id, spv::DecorationBinding);
            if (flags & rDescSet) compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
        }
    }

    inline std::unordered_map<std::string, int> uniformBuffersBindingIndex;
    inline uint32_t currentBindingIndex;

    inline void reIndexUniformBuffersBindingIndex(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources
    ) {
        for (const auto& resource : resources) {
            std::string name = compiler.get_name(compiler.get_type(resource.base_type_id).self);

            if (auto it = uniformBuffersBindingIndex.find(name); it != uniformBuffersBindingIndex.end()) {
                LOGW("The uniform buffer named %s already has a saved index! Overriding...", name.c_str());
            }
            
            uint32_t currentIndex = currentBindingIndex++;
            compiler.set_decoration(resource.id, spv::DecorationBinding, currentIndex);
            uniformBuffersBindingIndex[name] = currentIndex;
        }
    }

    inline void fixUniformBuffersBindingIndex(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources
    ) {
        if (uniformBuffersBindingIndex.empty()) return;
        for (const auto& resource : resources) {
            std::string name = compiler.get_name(compiler.get_type(resource.base_type_id).self);

            if (auto it = uniformBuffersBindingIndex.find(name); it == uniformBuffersBindingIndex.end()) {
                LOGW("The uniform buffer named %s doesn't have a saved index!", name.c_str());

                compiler.set_decoration(resource.id, spv::DecorationBinding, currentBindingIndex++);

                continue;
            }

            compiler.set_decoration(resource.id, spv::DecorationBinding, uniformBuffersBindingIndex.at(name));
        }
    }

    inline bool preprocessedVS, preprocessedFS;

    inline void processSPVBytecode(SPVCExposed_CompilerGLSL &compiler, shaderc_shader_kind kind) {
        if (kind == shaderc_glsl_compute_shader) {
            LOGI("Compute shader processing is unimplemented right now...");
            return;
        }

        if (!preprocessedVS && kind == shaderc_vertex_shader) preprocessedVS = true;
        if (!preprocessedFS && kind == shaderc_fragment_shader) preprocessedFS = true;

        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Process uniform samplers (textures)
        removeLocationBindingAndDescriptorSets(compiler, resources.separate_images);
        removeLocationBindingAndDescriptorSets(compiler, resources.sampled_images);
        removeLocationBindingAndDescriptorSets(compiler, resources.separate_samplers);

        // Process uniform buffers and potential standalone uniforms
        removeLocationBindingAndDescriptorSets(compiler, resources.uniform_buffers);

        if (kind == shaderc_fragment_shader)
            fixUniformBuffersBindingIndex(compiler, resources.uniform_buffers);
        else if (kind == shaderc_vertex_shader)
            reIndexUniformBuffersBindingIndex(compiler, resources.uniform_buffers);

        removeLocationBindingAndDescriptorSets(compiler, resources.gl_plain_uniforms);

        // Process shader inputs and outputs
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_inputs);
        
        int flags = rDescSet | rBinding | rLocation;
        if (kind == shaderc_fragment_shader
         && resources.stage_outputs.size() > 1) flags = rDescSet | rBinding;
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_outputs, flags);

        if (preprocessedVS && preprocessedVS) {
            uniformBuffersBindingIndex.clear();
            currentBindingIndex = 0;
        }
    }
}; // namespace ShaderConverer::SPVPostprocessor
