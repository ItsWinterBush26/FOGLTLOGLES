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
    // Maps to track various resource locations
    std::unordered_map<std::string, int> uniformLocationMap;
    uint32_t nextAvailableUniformLocation = 0;

    std::unordered_map<std::string, int> varyingLocationMap;
    uint32_t nextAvailableVaryingLocation = 0;

    std::unordered_map<std::string, int> attributeLocationMap;
    uint32_t nextAvailableAttributeLocation = 0;

    // Process uniform samplers (textures)
    void processSamplerUniforms(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &resource : resources) {
            std::string name = compiler.get_name(resource.id);
            if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                uniformLocationMap[name] = nextAvailableUniformLocation++;
            }
            int location = uniformLocationMap[name];
            compiler.set_decoration(resource.id, spv::DecorationLocation, location);
            compiler.set_decoration(resource.id, spv::DecorationBinding, location);
            LOGI("Sampler '%s' set to location/binding %d", name.c_str(), location);
        }
    }

    // Process uniform buffers and standalone uniforms within the uniform_buffers resource.
    // Standalone uniforms (declared outside of a block) will not have the Block decoration.
    void processUniformBuffersAndStandaloneUniforms(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &ubo : resources) {
            bool isBlock = compiler.has_decoration(ubo.id, spv::DecorationBlock);
            if (isBlock) {
                // Process as a true uniform block (UBO)
                std::string uboName = compiler.get_name(ubo.id);
                if (uniformLocationMap.find(uboName) == uniformLocationMap.end()) {
                    uniformLocationMap[uboName] = nextAvailableUniformLocation++;
                }
                int uboBinding = uniformLocationMap[uboName];
                compiler.set_decoration(ubo.id, spv::DecorationBinding, uboBinding);
                LOGI("UBO '%s' set to binding %d", uboName.c_str(), uboBinding);
                
                // Optionally process UBO members for debugging/tracking (locations not set in GLES)
                auto type = compiler.get_type(ubo.base_type_id);
                uint32_t memberCount = type.member_types.size();
                for (uint32_t i = 0; i < memberCount; ++i) {
                    std::string memberName = compiler.get_member_name(ubo.base_type_id, i);
                    std::string fullName = uboName + "." + memberName;
                    if (uniformLocationMap.find(fullName) == uniformLocationMap.end()) {
                        uniformLocationMap[fullName] = nextAvailableUniformLocation++;
                    }
                    LOGI("UBO member '%s' tracked (offset %u)",
                         fullName.c_str(),
                         compiler.get_member_decoration(ubo.base_type_id, i, spv::DecorationOffset));
                }
            } else {
                // This is a standalone uniform that happens to be in the uniform_buffers group.
                std::string name = compiler.get_name(ubo.id);
                if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                    uniformLocationMap[name] = nextAvailableUniformLocation++;
                }
                int location = uniformLocationMap[name];
                compiler.set_decoration(ubo.id, spv::DecorationLocation, location);
                LOGI("Standalone Uniform '%s' set to location %d", name.c_str(), location);
            }
        }
    }

    // Process plain uniforms that are declared outside of any block.
    // In recent SPIRV‑Cross versions these appear in gl_plain_uniforms.
    void processPlainUniforms(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &uniform : resources) {
            std::string name = compiler.get_name(uniform.id);
            if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                uniformLocationMap[name] = nextAvailableUniformLocation++;
            }
            int location = uniformLocationMap[name];
            compiler.set_decoration(uniform.id, spv::DecorationLocation, location);
            LOGI("Plain Uniform '%s' set to location %d", name.c_str(), location);
        }
    }

    // Process vertex attributes
    void processAttributes(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &attr : resources) {
            std::string name = compiler.get_name(attr.id);
            if (attributeLocationMap.find(name) == attributeLocationMap.end()) {
                attributeLocationMap[name] = nextAvailableAttributeLocation++;
            }
            int location = attributeLocationMap[name];
            compiler.set_decoration(attr.id, spv::DecorationLocation, location);
            LOGI("Attribute '%s' set to location %d", name.c_str(), location);
        }
    }

    // Process vertex shader outputs (varyings)
    void processVertexOutputs(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &varying : resources) {
            std::string name = compiler.get_name(varying.id);
            if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                varyingLocationMap[name] = nextAvailableVaryingLocation++;
            }
            int location = varyingLocationMap[name];
            compiler.set_decoration(varying.id, spv::DecorationLocation, location);
            LOGI("VS output varying '%s' set to location %d", name.c_str(), location);
        }
    }

    // Process fragment shader inputs (varyings)
    void processFragmentInputs(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &varying : resources) {
            std::string name = compiler.get_name(varying.id);
            if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                // Warn if a fragment input doesn’t match a vertex output.
                varyingLocationMap[name] = nextAvailableVaryingLocation++;
                LOGI("Warning: Fragment shader input '%s' not found in vertex outputs", name.c_str());
            }
            int location = varyingLocationMap[name];
            compiler.set_decoration(varying.id, spv::DecorationLocation, location);
            LOGI("FS input varying '%s' set to location %d", name.c_str(), location);
        }
    }

    // Process fragment shader outputs
    void processFragmentOutputs(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource> &resources
    ) {
        for (auto &output : resources) {
            std::string name = compiler.get_name(output.id);
            // For simplicity, setting all fragment outputs to location 0.
            compiler.set_decoration(output.id, spv::DecorationLocation, 0);
            LOGI("FS output '%s' set to location 0", name.c_str());
        }
    }

public:
    // Process the SPIRV bytecode, assigning resource locations based on shader stage.
    void processSPVBytecode(spirv_cross::CompilerGLSL &compiler, shaderc_shader_kind kind) {
        LOGI("Processing SPIRV bytecode for shader kind %d", kind);

        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Process uniform samplers (textures)
        processSamplerUniforms(compiler, resources.separate_images);
        processSamplerUniforms(compiler, resources.sampled_images);
        processSamplerUniforms(compiler, resources.separate_samplers);

        // Process uniform buffers and potential standalone uniforms
        processUniformBuffersAndStandaloneUniforms(compiler, resources.uniform_buffers);

        // Process uniforms declared outside any block using gl_plain_uniforms
        processPlainUniforms(compiler, resources.gl_plain_uniforms);

        // Process shader stage-specific resources
        switch (kind) {
            case shaderc_vertex_shader:
                processAttributes(compiler, resources.stage_inputs);
                processVertexOutputs(compiler, resources.stage_outputs);
                break;
            case shaderc_fragment_shader:
                processFragmentInputs(compiler, resources.stage_inputs);
                processFragmentOutputs(compiler, resources.stage_outputs);
                break;
            case shaderc_compute_shader:
                LOGI("Compute shader processing - special handling may be required");
                break;
            default:
                LOGI("Unsupported shader kind %d for resource processing", kind);
                break;
        }

        LOGI("Processing complete: %u uniforms, %u varyings, %u attributes", 
             nextAvailableUniformLocation, 
             nextAvailableVaryingLocation,
             nextAvailableAttributeLocation);
    }

    // Reset the preprocessor state.
    void reset() {
        uniformLocationMap.clear();
        varyingLocationMap.clear();
        attributeLocationMap.clear();
        nextAvailableUniformLocation = 0;
        nextAvailableVaryingLocation = 0;
        nextAvailableAttributeLocation = 0;
        LOGI("Preprocessor state reset");
    }
};
