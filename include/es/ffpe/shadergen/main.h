#pragma once

#include "es/ffpe/shadergen/cache.h"
#include "es/ffpe/shadergen/alphatest.h"
#include "es/state_tracking.h"
#include "fmt/format.h"
#include "gles/ffp/enums.h"
#include "gles/main.h"
#include "gles20/shader_overrides.h"
#include "shaderc/shaderc.h"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::ShaderGen {

inline const std::string SG_NEWLINE = "\n\n";

inline const std::string VS_TEMPLATE = R"(#version 320 es
// FOGLTLOGLES ShaderGen : Vertex Shader

// default inputs
layout(location = 0) in mediump vec4 iVertexPosition;
layout(location = 1) in lowp vec4 iVertexColor;
layout(location = 2) in mediump vec4 iVertexTexCoord;

uniform mat4 uModelViewProjection;

// default outputs
flat out lowp int vertexID;
out lowp vec4 vertexColor;
out mediump vec4 vertexTexCoord;

void main() {
    gl_Position = iVertexPosition * uModelViewProjection;

    vertexID = gl_VertexID;
    vertexColor = iVertexColor:
    verexTexCoord = iVertexTexCoord;
})";

inline const std::string FS_TEMPLATE = R"(#version 320 es
precision mediump float;

// FOGLTLOGLES ShaderGen : Fragment Shader

// default inputs
flat in lowp int vertexID;
in lowp vec4 vertexColor;
in mediump vec4 vertexTexCoord;

// feature inputs
{}

// default output/s
out lowp vec4 oFragColor;

// feature outputs
{}

void main() {{
    lowp vec4 color = vertexColor;

    // feature operations
    {}

    oFragColor = color;
}})";

inline std::string buildVertexShader() {
    return "";
}

inline std::string buildFragmentShader() {
    std::string inputs;
    std::string outputs;
    std::string operations;

    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        alphaTest.build(inputs, outputs, operations);
    }

    return fmt::format(
        FS_TEMPLATE,
        inputs, outputs, operations
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

    Cache::Programs::putToCache(state, renderingProgram);
    return renderingProgram;
}

}