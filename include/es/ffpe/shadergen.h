#pragma once

#include "es/ffp.h"
#include "es/state_tracking.h"
#include "gles/ffp/enums.h"
#include <GLES3/gl32.h>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

namespace FFPE::ImmediateMode::ShaderGen {

inline const std::string nextLine = "\n\n";

inline const std::regex bindPointRegex("\{\{BIND_POINT\}\}");

inline uint32_t nextBindingPointSSBO = 0;

inline const std::string immediateModeTemplateVS = R"(#version 320 es

uniform mat4 uModelViewProjection;

layout(location = 0) in vec4 iVertexPosition;
layout(location = 1) in vec3 iVertexNormal; // not used, to be used in the future
layout(location = 2) in vec4 iVertexColor;

out vec4 vertexColor;

void main() {
    gl_Position = uModelViewProjection * iVertexPosition;
    vertexColor = iVertexColor;
})";

inline const std::string immediateModeTemplateFS = R"(#version 320 es

in vec4 vertexColor;

{{FS_SSBO}}

void main() {
   vec4 color = vertexColor;

   {{FS_IMPL}}

   fragColor = color;
})";

/*
inline std::string generateShaderVS() {

}


inline std::pair<std::string, std::unordered_map<std::string, uint32_t>> generateShaderFS() {
    std::unordered_map<std::string, uint32_t> bindPoints;

    std::stringstream shaderSSBO;
    std::stringstream shaderIMPL;

    if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) {
        uint32_t bindPoint = nextBindingPointSSBO++;
        std::string evaluatedSSBO = std::regex_replace(
            FFPE::ImmediateMode::ShaderGen::MultiTexCoord::multiTexCoordSSBO,
            bindPointRegex, bindPoint
        );

        shaderSSBO << evaluatedSSBO << nextLine;
        bindPoints.insert({ "multitexcoord", bindPoint });
    }

    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        uint32_t bindPoint = nextBindingPointSSBO++;
        std::string evaluatedSSBO = std::regex_replace(
            FFPE::ImmediateMode::ShaderGen::AlphaTest::alphaTestSSBO,
            bindPointRegex, bindPoint
        );
        
        shaderSSBO << evaluatedSSBO << nextLine;
        bindPoints.insert({ "alphatest", bindPoint });

        shaderIMPL << FFPE::ImmediateMode::ShaderGen::AlphaTest::alphaTestIMPL << nextLine;
    }
} */

namespace MultiTexCoord {

inline const std::string multiTexCoordSSBO = R"(layout(std430, binding = {{BIND_POINT}}) readonly buffer MultiTexCoord {

    vec2 multiTexCoords[64];
};)";

inline const std::string multiTexCoordIMPL = R"(color *= texture(uTexture{{TEX_UNIT}}, multiTexCoords[{{TEX_UNIT}}]))";

}

namespace AlphaTest {

inline const std::string alphaTestSSBO = R"(layout(std430, binding = {{BIND_POINT}}) readonly buffer AlphaTesting {
    int alphaTestingOp;
    float alplaTestingThreshold;
};)";

inline const std::string alphaTestIMPL = R"(if (alphaTestingOp != 0x0207) {
    if (alphaTestingOp == 0x0200) discard;
    bool alphaTestingPassed = false;
    float colorOpacity = color.a;

    if (alphaTestingOp == 0x0201)
    else if ()
    else if ()
    else if ()
    else if ()
    else if ()
    else if ()

    if (!alphaTestingPassed) discard;
})";

}
}
