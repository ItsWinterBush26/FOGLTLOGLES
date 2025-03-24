#pragma once

#include "spirv.hpp"
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <shaderc/shaderc.hpp>

class SPVCExposed_CompilerGLSL : public spirv_cross::CompilerGLSL {
    public:
        using spirv_cross::CompilerGLSL::CompilerGLSL;
        using spirv_cross::CompilerGLSL::get;
};

enum {
    rLocation = 1 << 1,
    rBinding = 1 << 2,
    rDescSet = 1 << 3,
    rDefault = rLocation | rBinding | rDescSet
};

namespace ShaderConverter::SPVCPostprocessor {
    inline void removeLocationBindingAndDescriptorSets(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources,
        int flags = rDefault
    ) {
        for (const auto& resource : resources) {
            compiler.unset_decoration(resource.id, spv::DecorationLocation);
            compiler.unset_decoration(resource.id, spv::DecorationBinding);
            compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
        }
    }

    inline void processSPVBytecode(SPVCExposed_CompilerGLSL &compiler, shaderc_shader_kind kind) {
        if (kind == shaderc_glsl_compute_shader) {
            LOGI("Compute shader processing is unimplemented right now...");
            return;
        }

        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Process uniform samplers (textures)
        removeLocationBindingAndDescriptorSets(compiler, resources.separate_images);
        removeLocationBindingAndDescriptorSets(compiler, resources.sampled_images);
        removeLocationBindingAndDescriptorSets(compiler, resources.separate_samplers);

        // Process uniform buffers and potential standalone uniforms
        removeLocationBindingAndDescriptorSets(compiler, resources.uniform_buffers);
        removeLocationBindingAndDescriptorSets(compiler, resources.gl_plain_uniforms);

        // Process shader inputs and outputs
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_inputs);
        
        int flags = rDescSet | rBinding;
        if (kind != shaderc_fragment_shader
         && resources.stage_outputs.size() < 2) flags |= rLocation;
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_outputs, flags);
    }
}; // namespace ShaderConverer::SPVPostprocessor
