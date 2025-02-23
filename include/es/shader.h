#pragma once

#include "es/utils.h"
#include "gl/shader.h"
#include "shaderc/shaderc.h"
#include "shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "utils/log.h"

#include <GLES3/gl31.h>
#include <regex>
#include <stdexcept>
#include <string>

namespace ESUtils {
    static inline void combineSources(GLsizei count, const GLchar *const* sources, const GLint* length, std::string& destination) {
        for (GLsizei i = 0; i < count; i++) {
            if (sources[i]) {
               if (length && length[i] > 0) {
                     destination.append(sources[i], length[i]);
               } else {
                   destination.append(sources[i]);
               }
           }
       }
    }

    static inline shaderc_shader_kind getKindFromShader(GLuint& shader) {
        GLint shaderType;
        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        
        switch (shaderType) {
            case GL_FRAGMENT_SHADER:
                return shaderc_fragment_shader;
            case GL_VERTEX_SHADER:
                return shaderc_vertex_shader;
            case GL_COMPUTE_SHADER:
                if (version.first == 3 && version.second >= 1) return shaderc_compute_shader;
                throw std::runtime_error("You need OpenGL ES 3.1 or newer for compute shaders!");
            
            default:
                LOGI("%u", shader);
                throw std::runtime_error("Received an unsupported shader type!");

        }
    }

    static inline void replaceShaderVersion(std::string& shaderSource, const std::string& newVersion) {
        std::regex versionRegex(R"(#version\s+\d{3}(\s+\w+)?\b)");  // Ensures full match
        shaderSource = std::regex_replace(shaderSource, versionRegex, "#version " + newVersion + "\n");
    }

    static inline void glslToEssl(shaderc_shader_kind kind, std::string& fullSource) {
        LOGI("GLSL to SPIR-V starting now...");

        int glslVersion = 0;
        if (sscanf(fullSource.c_str(), "#version %i", &glslVersion) != 1) {
            throw new std::runtime_error("No '#version XXX' preprocessor!");
        }

        LOGI("Detected shader GLSL version is %i", glslVersion);
        if (glslVersion < 330) {
            glslVersion = 330;
            LOGI("Seems like we need to upgrade to GLSL 330... Kidding.");

            // upgradeTo330(kind, fullSource);
            replaceShaderVersion(fullSource, "330");

            LOGI("New shader GLSL version is %i", glslVersion);
        }

        shaderc::CompileOptions spirvOptions;
        spirvOptions.SetSourceLanguage(shaderc_source_language_glsl);
        spirvOptions.SetTargetEnvironment(shaderc_target_env_opengl, glslVersion);
        spirvOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

        spirvOptions.SetAutoMapLocations(true);
        spirvOptions.SetAutoBindUniforms(true);
        spirvOptions.SetAutoSampledTextures(true);

        shaderc::Compiler spirvCompiler;
        shaderc::SpvCompilationResult bytecode = spirvCompiler.CompileGlslToSpv(
            fullSource, kind,
            "shader", spirvOptions
        );

        if (bytecode.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::string errorMessage = bytecode.GetErrorMessage();
            throw std::runtime_error("Shader compilation error: " + errorMessage);
        }

        LOGI("GLSL to SPIR-V succeeded! Commencing stage 2...");

        spirv_cross::CompilerGLSL::Options esslOptions;
        esslOptions.version = shadingVersion;
        esslOptions.es = true;
        esslOptions.vulkan_semantics = false;

        esslOptions.force_flattened_io_blocks = true;
   
        spirv_cross::CompilerGLSL esslCompiler({ bytecode.cbegin(), bytecode.cend() });
        esslCompiler.set_common_options(esslOptions);

        fullSource = esslCompiler.compile();

        LOGI("SPIR-V to ESSL succeeded!");
    }
}
