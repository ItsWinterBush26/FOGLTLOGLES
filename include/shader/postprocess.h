#pragma once

#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <shaderc/shaderc.hpp>

class PostProcessor {
private:
    void removeLocationBindingAndDescriptorSets(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &resource : resources) {
            compiler.unset_decoration(resource.id, spv::DecorationLocation);
            compiler.unset_decoration(resource.id, spv::DecorationBinding);
            compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
        }
    }

public:
    void processSPVBytecode(spirv_cross::CompilerGLSL &compiler, shaderc_shader_kind kind) {
        LOGI("Processing SPIRV bytecode for %s", getKindStringFromKind(kind));
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
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_outputs);
   
        LOGI("Processing complete!");
    }
};
