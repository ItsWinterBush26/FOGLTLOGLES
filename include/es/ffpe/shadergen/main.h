#pragma once

#include "es/ffpe/shadergen/cache.h"
#include "es/ffpe/shadergen/common.h"
#include "es/ffpe/shadergen/features/alphatest.h"
#include "es/ffpe/shadergen/features/texture.h"
#include "es/state_tracking.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "gles/ffp/enums.h"
#include "gles/main.h"
#include "gles20/shader_overrides.h"
#include "shaderc/shaderc.h"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::ShaderGen {

inline std::string buildVertexShader() {
    std::stringstream inputs;
    std::stringstream outputs;
    std::stringstream operations;

    // TODO: do a register for feature type impl, so we can just loop on a vector or something
    if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) {
        Feature::Texture::instance.buildVS(inputs, outputs, operations);
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

    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        Feature::AlphaTest::instance.buildFS(inputs, outputs, operations);
    }

    if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) {
        Feature::Texture::instance.buildFS(inputs, outputs, operations);
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

}
