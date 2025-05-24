#pragma once

#include "es/ffp.h"
#include "es/ffpe/uniforms.h"
#include "es/state_tracking.h"
#include "fmt/base.h"
#include "gles/ffp/enums.h"
#include "gles/main.h"
#include "gles20/shader_overrides.h"
#include "utils/fast_map.h"
#include "utils/log.h"
#include "utils/span.h"

#include <fmt/format.h>
#include <GLES3/gl32.h>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

namespace FFPE::Rendering::ShaderGen {

namespace States {
    inline GLbitfield currentState;
    inline GLuint currentProgram;
    inline std::pair<
    	std::unordered_map<std::string, uint32_t>,
    	std::unordered_map<std::string, uint32_t>
    > programBindings;

namespace Cache {
    inline FastMapBI<GLbitfield, GLuint> cachedPrograms;
}

}

namespace MultiTexCoord {

// WHAT IF:
// pass ssbo to vertexShader
// pass a vec2 array containing vec2s for each texcoord array of each active 

// main bottleneck is probaby gonna be texCoords[];

inline const std::string multiTexCoordSSBO1 = R"(layout(std430, binding = {}) readonly buffer ActiveUnits {{
	uint activeUnitsSize;
    uint activeUnits[];
}};)";

inline const std::string multiTexCoordSSBO2 = R"(layout(std430, binding = {}) readonly buffer TexCoords {{
	uint texCoordsSize; // size of each array
	vec2 texCoords[]; // 1d contiguous array of texcoords e.g : allUnit1texcoords | allUnit(i+1)texcoords | ...;
}};)";

inline const std::string multiTexCoordUNIF = "uniform sampler2D textures[32]"; // 32 comes from spec, minimum available tex units

inline const std::string multiTexCoordIMPL = R"(
	for (uint i = 0; i < activeUnitsSize; ++i) {
		uint texCoordIndex = (i * texCoordsSize) + vertexID;
		color *= texture(textures[activeUnits[i]], texCoords[texCoordIndex]);
	}
)";

inline GLuint activeUnitsSSBO, texCoordsSSBO;

inline void init() {
	glGenBuffers(1, &activeUnitsSSBO);
	glGenBuffers(2, &texCoordsSSBO);
}

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

// TODO: since vecN, where N is the component
// and it might change, account for that
// GL_*_ARRAY.size should do
inline const std::string renderingShaderTemplateVS = R"(#version 320 es

uniform mat4 uModelViewProjection;

layout(location = 0) in mediump vec4 iVertexPosition;
layout(location = 1) in lowp vec4 iVertexColor;
layout(location = 2) in mediump vec4 iVertexTexCoord;

out lowp int vertexID;
out lowp vec4 vertexColor;
out mediump vec4 vertexTexCoord;

void main() {
    gl_Position = iVertexPosition * uModelViewProjection;

	vertexID = gl_VertexID;
    vertexColor = iVertexColor;
    vertexTexCoord = iVertexTexCoord;
})";

inline const std::string renderingShaderTemplateFS = R"(#version 320 es
precision mediump float;

in lowp int vertexID;
in lowp vec4 vertexColor;
in mediump vec4 vertexTexCoord;

out lowp vec4 oFragColor;

// SSBO
{}

// UNIFORM
{}

void main() {{
   vec4 color = vertexColor;

   {}

   oFragColor = color;
}})";

inline void init() {
	MultiTexCoord::init();
}

inline std::pair<std::string, std::unordered_map<std::string, uint32_t>> generateShaderVS() {
    return { renderingShaderTemplateVS, std::unordered_map<std::string, uint32_t>() };
}

inline std::pair<std::string, std::unordered_map<std::string, uint32_t>> generateShaderFS() {
    std::unordered_map<std::string, uint32_t> bindPoints;

    std::string shaderSSBO;
	std::string shaderUNIF;
    std::string shaderIMPL;

    /* if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) {
        uint32_t bindPoint1 = nextBindingPointSSBO++;
        fmt::format_to(
            std::back_inserter(shaderSSBO),
            MultiTexCoord::multiTexCoordSSBO1, bindPoint1
        );

		uint32_t bindPoint2 = nextBindingPointSSBO++;
        fmt::format_to(
            std::back_inserter(shaderSSBO),
            MultiTexCoord::multiTexCoordSSBO2, bindPoint2
        );

		shaderUNIF += MultiTexCoord::multiTexCoordUNIF + nextLine;
		shaderIMPL += MultiTexCoord::multiTexCoordIMPL + nextLine;

        bindPoints.insert({ "activeunits", bindPoint1 });
        bindPoints.insert({ "texcoords", bindPoint2 });
    } */

    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        AlphaTest::generateAlphaTestIMPL(shaderIMPL);
    }

    std::string finalShader;
    fmt::format_to(
        std::back_inserter(finalShader),
        renderingShaderTemplateFS,
        shaderSSBO, shaderUNIF, shaderIMPL
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
        LOGI("Generated shader for state '%u'", state);

        LOGI("ShaderGen | VS :");
        LOGI("%s", vertexShaderSource.first.c_str());

        LOGI("ShaderGen | FS :");
        LOGI("%s", fragmentShaderSource.first.c_str());
    }
	
    States::currentState = state;
    States::currentProgram = renderingProgram;
	States::programBindings = { vertexShaderSource.second, fragmentShaderSource.second };
    States::Cache::cachedPrograms.insert({ state, renderingProgram });

    return renderingProgram;
}

inline void setupUniformsAndSSBO() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, MultiTexCoord::activeUnitsSSBO);
	glBufferData(
		GL_SHADER_STORAGE_BUFFER,
		FFPE::States::ClientState::texCoordArrayTexUnits.size() * sizeof(GLenum),
        FFPE::States::ClientState::texCoordArrayTexUnits.data(),
		GL_DYNAMIC_DRAW
	);
}

}
