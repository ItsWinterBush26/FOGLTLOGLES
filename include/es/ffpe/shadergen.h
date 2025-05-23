#pragma once

#include "es/ffp.h"
#include "es/state_tracking.h"
#include "fmt/base.h"
#include "gles/ffp/enums.h"
#include "gles/main.h"
#include "gles20/shader_overrides.h"
#include "utils/fast_map.h"
#include "utils/log.h"

#include <fmt/format.h>
#include <GLES3/gl32.h>
#include <iterator>
#include <string>
#include <unordered_map>

namespace FFPE::Rendering::ShaderGen {

namespace States {
    inline GLbitfield currentState;
    inline GLuint currentProgram;

namespace Cache {
    inline FastMapBI<GLbitfield, GLuint> cachedPrograms;
}

}

namespace MultiTexCoord {

inline const std::string multiTexCoordSSBO = R"(layout(std430, binding = {}) readonly buffer MultiTexCoord {
    vec2 multiTexCoords[64];
};)";

inline const std::string multiTexCoordIMPL = "color *= texture(uTexture{}, multiTexCoords[{}])";

}

namespace AlphaTest {

inline const std::string alphaTestIMPLTemplate = "if(color.a {} {}) discard;";

inline void generateAlphaTestIMPL(std::string& output) {
    std::string operation;

    switch (FFPE::States::AlphaTest::op) {
        case GL_LESS:
            operation = "<";
        break;

        case GL_EQUAL:
            operation = "==";
        break;

        case GL_LEQUAL:
            operation = "<=";
        break;

        case GL_GREATER:
            operation = ">";
        break;

        case GL_NOTEQUAL:
            operation = "!=";
        break;

        case GL_GEQUAL:
            operation = ">=";
        break;

        case GL_NEVER: output = "discard;"; return;
        case GL_ALWAYS: return;
    }

    fmt::format_to(
        std::back_inserter(output),
        alphaTestIMPLTemplate,
        operation,
        std::to_string(FFPE::States::AlphaTest::threshold)
    );
}

}

inline const std::string nextLine = "\n\n";

inline uint32_t nextBindingPointSSBO = 0;

inline const std::string renderingShaderTemplateVS = R"(#version 320 es

uniform mat4 uModelViewProjection;

layout(location = 0) in vec4 iVertexPosition;
layout(location = 1) in vec4 iVertexColor;
layout(location = 3) in vec4 iVertexTexCoord;

out vec4 vertexColor;
out vec4 vertexTexCoord;

void main() {
    gl_Position = iVertexPosition * uModelViewProjection;

    vertexColor = iVertexColor;
    vertexTexCoord = iVertexTexCoord;
})";

inline const std::string renderingShaderTemplateFS = R"(#version 320 es

in vec4 vertexColor;

{0}

void main() {
   vec4 color = vertexColor;

   {1}

   fragColor = color;
})";

inline std::pair<std::string, std::unordered_map<std::string, uint32_t>> generateShaderVS() {
    return { };
}

inline std::pair<std::string, std::unordered_map<std::string, uint32_t>> generateShaderFS() {
    std::unordered_map<std::string, uint32_t> bindPoints;

    std::string shaderSSBO;
    std::string shaderIMPL;

    /* if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) {
        uint32_t bindPoint = nextBindingPointSSBO++;
        fmt::format_to(
            std::back_inserter(shaderSSBO),
            MultiTexCoord::multiTexCoordSSBO, bindPoint
        );

        bindPoints.insert({ "multitexcoord", bindPoint });
    } */

    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        AlphaTest::generateAlphaTestIMPL(shaderIMPL);
    }

    std::string finalShader;
    fmt::format_to(
        std::back_inserter(finalShader),
        renderingShaderTemplateFS,
        shaderSSBO, shaderIMPL
    );

    return { finalShader, bindPoints };
}

inline GLuint getCachedOrBuildProgram(GLbitfield state) {
    if (States::currentState == state) return States::currentProgram;
    
    auto cachedProgram = States::Cache::cachedPrograms.find(state);
    if (cachedProgram != States::Cache::cachedPrograms.end()) return cachedProgram->second;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    auto vertexShaderSource = generateShaderVS();
    const GLchar* convertedVS = vertexShaderSource.first.c_str();
    OV_glShaderSource(vertexShader, 1, &convertedVS, nullptr);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    auto fragmentShaderSource = generateShaderFS();
    const GLchar* convertedFS = fragmentShaderSource.first.c_str();
    OV_glShaderSource(fragmentShader, 1, &convertedFS, nullptr);
    
    OV_glCompileShader(vertexShader);
    OV_glCompileShader(fragmentShader);

    GLuint renderingProgram = glCreateProgram();
    glAttachShader(renderingProgram, vertexShader);
    glAttachShader(renderingProgram, fragmentShader);

    OV_glLinkProgram(renderingProgram);

    if (debugEnabled) {
        LOGI("ShaderGen | VS :");
        LOGI("%s", vertexShaderSource.first.c_str());
    }

    States::currentState = state;
    States::currentProgram = renderingProgram;
    States::Cache::cachedPrograms.insert({ state, renderingProgram });

    return renderingProgram;
}

}
