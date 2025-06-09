#pragma once

#include "es/ffpe/shadergen/cache.hpp"
#include "es/ffpe/shadergen/common.hpp"
#include "es/ffpe/shadergen/features/alphatest.hpp"
#include "es/ffpe/shadergen/features/mvp.hpp"
#include "es/ffpe/shadergen/features/registry.hpp"
#include "es/ffpe/shadergen/features/vertexbuffer.hpp"
#include "fmt/base.h"
#include "fmt/format.h"
#include "gles/ffp/enums.hpp"
#include "gles/main.hpp"
#include "gles20/shader_overrides.hpp"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::ShaderGen {

inline std::string buildVertexShader() {
    std::stringstream inputs;
    std::stringstream outputs;
    std::stringstream operations;

    for (const auto* feature : Feature::Registry::Data::registeredFeatures) {
        feature->buildVS(inputs, outputs, operations);
    }

    return fmt::format(
        fmt::runtime(Common::VS_TEMPLATE),
        inputs.str(), outputs.str(), operations.str()
    );
}

inline std::string buildFragmentShader() {
    std::stringstream inputs;
    std::stringstream outputs;
    std::stringstream operations;

    for (const auto* feature : Feature::Registry::Data::registeredFeatures) {
        feature->buildFS(inputs, outputs, operations);
    }

    return fmt::format(
        fmt::runtime(Common::FS_TEMPLATE),
        inputs.str(), outputs.str(), operations.str()
    );
}

inline GLuint getCachedOrNewProgram(GLbitfield64 state) {
    GLuint cachedProgram = Cache::Programs::getFromCache(state);
    if (cachedProgram) return cachedProgram;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexSource = Cache::Shaders::getFromCache(state, shaderc_vertex_shader);
    if (vertexSource.empty()) {
        vertexSource = buildVertexShader();
        Cache::Shaders::putToCache(state, shaderc_vertex_shader, vertexSource);
    }

    const GLchar* convertedVS = vertexSource.c_str();
    glShaderSource(
        vertexShader, 1,
        &convertedVS, nullptr
    );

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentSource = Cache::Shaders::getFromCache(state, shaderc_fragment_shader);
    if (fragmentSource.empty()) {
        fragmentSource = buildFragmentShader();
        Cache::Shaders::putToCache(state, shaderc_fragment_shader, fragmentSource);
    }

    const GLchar* convertedFS = fragmentSource.c_str();
    glShaderSource(
        fragmentShader, 1,
        &convertedFS, nullptr
    );

    OV_glCompileShader(vertexShader);
    OV_glCompileShader(fragmentShader);

    if (debugEnabled) {
        LOGI("Generated shader for state '%ld'", state);

        LOGI("ShaderGen | VS :");
        LOGI("%s", vertexSource.c_str());

        LOGI("ShaderGen | FS :");
        LOGI("%s", fragmentSource.c_str());
    }

    GLuint renderingProgram = glCreateProgram();
    glAttachShader(renderingProgram, vertexShader);
    glAttachShader(renderingProgram, fragmentShader);

    OV_glLinkProgram(renderingProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Cache::Programs::putToCache(state, renderingProgram);
    return renderingProgram;
}

inline void initFeatures() {
    // NOTE: order matters!
    Feature::Registry::registerFeature<Feature::VertexAttrib::VertexAttribFeature>();
    Feature::Registry::registerFeature<Feature::MVP::MVPFeature>();
    // Feature::Registry::registerFeature<Feature::Texture::TextureFeature>();

    Feature::Registry::registerFeature<Feature::AlphaTest::AlphaTestFeature>();
}

}
