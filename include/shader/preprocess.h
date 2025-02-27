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
    // Maps to track uniform and varying locations across compilation units
    std::unordered_map<std::string, int> uniformLocationMap;
    uint32_t nextAvailableUniformLocation = 0;

    std::unordered_map<std::string, int> varyingLocationMap;
    uint32_t nextAvailableVaryingLocation = 0;

    // Maps for attribute locations
    std::unordered_map<std::string, int> attributeLocationMap;
    uint32_t nextAvailableAttributeLocation = 0;

    void processSamplerUniforms(
        spirv_cross::CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& separateImages
    ) {
        for (auto& img : separateImages) {
            std::string name = compiler.get_name(img.id);
            
            // For sampler uniforms, we need to set both location and binding
            if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                uniformLocationMap[name] = nextAvailableUniformLocation++;
            }
            
            int location = uniformLocationMap[name];
            compiler.set_decoration(img.id, spv::DecorationLocation, location);
            
            // Also set binding for samplers to match location
            compiler.set_decoration(img.id, spv::DecorationBinding, location);
            
            LOGI("Sampler '%s' set to location/binding %d", name.c_str(), location);
        }
    }

    void processUniformBuffers(
        spirv_cross::CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& uniformBuffers
    ) {
        for (auto &ubo : uniformBuffers) {
            std::string uboName = compiler.get_name(ubo.id);
            
            // Set binding for the UBO itself
            if (uniformLocationMap.find(uboName) == uniformLocationMap.end()) {
                uniformLocationMap[uboName] = nextAvailableUniformLocation++;
            }
            int uboBinding = uniformLocationMap[uboName];
            compiler.set_decoration(ubo.id, spv::DecorationBinding, uboBinding);
            
            LOGI("UBO '%s' set to binding %d", uboName.c_str(), uboBinding);
            
            // Process UBO members if needed
            auto type = compiler.get_type(ubo.base_type_id);
            uint32_t memberCount = type.member_types.size();
            
            for (uint32_t i = 0; i < memberCount; ++i) {
                std::string memberName = compiler.get_member_name(ubo.base_type_id, i);
                std::string fullName = uboName + "." + memberName;
                
                // We don't set locations for UBO members in GLES
                // Just track them for debugging purposes
                if (uniformLocationMap.find(fullName) == uniformLocationMap.end()) {
                    uniformLocationMap[fullName] = nextAvailableUniformLocation++;
                }
                
                LOGI("UBO member '%s' tracked (offset %u)", 
                    fullName.c_str(), 
                    compiler.get_member_decoration(ubo.base_type_id, i, spv::DecorationOffset));
            }
        } 
    }

    void processAttributes(
        spirv_cross::CompilerGLSL& compiler,
        const spirv_cross::SmallVector<spirv_cross::Resource>& inputs
    ) {
        for (auto &attr : inputs) {
            std::string name = compiler.get_name(attr.id);
            
            if (attributeLocationMap.find(name) == attributeLocationMap.end()) {
                attributeLocationMap[name] = nextAvailableAttributeLocation++;
            }
            
            int location = attributeLocationMap[name];
            compiler.set_decoration(attr.id, spv::DecorationLocation, location);
            
            LOGI("Attribute '%s' set to location %d", name.c_str(), location);
        }
    }

    void processVaryings(
        spirv_cross::CompilerGLSL &compiler,
        const spirv_cross::ShaderResources &resources,
        shaderc_shader_kind kind
    ) {
        switch (kind) {
            case shaderc_vertex_shader: {
                // Stage outputs in vertex shader are varyings
                for (auto &var : resources.stage_outputs) {
                    std::string name = compiler.get_name(var.id);
                    
                    if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                        varyingLocationMap[name] = nextAvailableVaryingLocation++;
                    }
                    
                    int location = varyingLocationMap[name];
                    compiler.set_decoration(var.id, spv::DecorationLocation, location);
                    
                    LOGI("VS output varying '%s' set to location %d", name.c_str(), location);
                }
                
                // Process vertex attributes (inputs)
                processAttributes(compiler, resources.stage_inputs);
                break;
            }
            
            case shaderc_fragment_shader: {
                // Stage inputs in fragment shader are varyings (must match vertex shader outputs)
                for (auto &var : resources.stage_inputs) {
                    std::string name = compiler.get_name(var.id);
                    
                    if (varyingLocationMap.find(name) == varyingLocationMap.end()) {
                        varyingLocationMap[name] = nextAvailableVaryingLocation++;
                        LOGI("Warning: Fragment shader input '%s' not found in vertex outputs", name.c_str());
                    }
                    
                    int location = varyingLocationMap[name];
                    compiler.set_decoration(var.id, spv::DecorationLocation, location);
                    
                    LOGI("FS input varying '%s' set to location %d", name.c_str(), location);
                }
                
                // Handle fragment shader outputs (usually gl_FragColor in ES)
                for (auto &var : resources.stage_outputs) {
                    std::string name = compiler.get_name(var.id);
                    compiler.set_decoration(var.id, spv::DecorationLocation, 0);
                    LOGI("FS output '%s' set to location 0", name.c_str());
                }
                break;
            }
            
            default:
                LOGI("Unsupported shader kind for varying processing");
                break;
        }
    }

public:
    void processSPVBytecode(spirv_cross::CompilerGLSL& compiler, shaderc_shader_kind kind) {
        LOGI("Processing SPIRV bytecode for shader kind %d", kind);

        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Process different resource types
        processSamplerUniforms(compiler, resources.separate_images);
        processUniformBuffers(compiler, resources.uniform_buffers);
        processVaryings(compiler, resources, kind);

        // Handle other uniform types
        for (auto &uniform : resources.separate_samplers) {
            std::string name = compiler.get_name(uniform.id);
            if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                uniformLocationMap[name] = nextAvailableUniformLocation++;
            }
            compiler.set_decoration(uniform.id, spv::DecorationLocation, uniformLocationMap[name]);
            compiler.set_decoration(uniform.id, spv::DecorationBinding, uniformLocationMap[name]);
            LOGI("Separate sampler '%s' set to location/binding %d", name.c_str(), uniformLocationMap[name]);
        }

        // Other standalone uniforms
        for (auto &uniform : resources.separate_constants) {
            std::string name = compiler.get_name(uniform.id);
            if (uniformLocationMap.find(name) == uniformLocationMap.end()) {
                uniformLocationMap[name] = nextAvailableUniformLocation++;
            }
            compiler.set_decoration(uniform.id, spv::DecorationLocation, uniformLocationMap[name]);
            LOGI("Constant '%s' set to location %d", name.c_str(), uniformLocationMap[name]);
        }

        LOGI("Processing complete: %u uniforms, %u varyings, %u attributes", 
            nextAvailableUniformLocation, 
            nextAvailableVaryingLocation,
            nextAvailableAttributeLocation);
    }

    // Reset preprocessor state
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
