#pragma once

#include "shader/utils.h"
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <shaderc/shaderc.hpp>

class SPVCExposed_CompilerGLSL : public spirv_cross::CompilerGLSL {
    public:
        using spirv_cross::CompilerGLSL::CompilerGLSL;
        using spirv_cross::CompilerGLSL::get;
};


namespace ShaderConverter::SPVPostprocessor {
    inline void removeLocationBindingAndDescriptorSets(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources
    ) {
        for (const auto& resource : resources) {
            compiler.unset_decoration(resource.id, spv::DecorationLocation);
            compiler.unset_decoration(resource.id, spv::DecorationBinding);
            compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
        }
    }

    inline void removeInitializers(
        SPVCExposed_CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& resources
    ) {
        for (const auto& resource : resources) {
            auto& var = compiler.get<spirv_cross::SPIRVariable>(resource.id);
            if (var.initializer != 0) var.initializer = 0;
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
        removeLocationBindingAndDescriptorSets(compiler, resources.stage_outputs);

        // removeInitializers(compiler, resources.uniform_buffers);
        // removeInitializers(compiler, resources.storage_buffers);
    }
}; // namespace ShaderConverer::SPVPostprocessor

namespace ShaderConverter::RegexPostprocessor {
    // I JUST HAD TO DO IT 💀

    std::regex patternMat4(R"(uniform\s+mat4\s+(\w+)\s*=\s*mat4\s*\([^)]*\)\s*;)");

    inline void removeInitializersMat4(std::string& source) {
        source = std::regex_replace(source, patternMat4, "uniform mat4 $1;");
    }

    inline void processESSLSource(std::string& source) {
        removeInitializersMat4(source);
    }
}
