#pragma once

#include "es/ffp.h"
#include "es/ffpe/shadergen/feature.h"
#include "es/ffpe/shadergen/shadergen.h"
#include "es/ffpe/uniforms.h"
#include "fmt/format.h"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::ShaderGen {
struct AlphaTestFeature : public Feature::BaseFeature {

const std::string uniforms = "uniform float alphaTestThreshold;";

const std::string baseOperation = "if (color.a {} alphaTestThreshold) discard;";

void build(
    [[maybe_unused]] std::string& finalInputs,
    [[maybe_unused]] std::string& finalOutputs,
    std::string& finalOperations
) override {
    std::string operation;

    switch (States::AlphaTest::op) {
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

        case GL_NEVER: finalOperations += "discard;"; return;
        case GL_ALWAYS: return;
    }

    fmt::format_to(
        std::back_inserter(finalOperations),
        baseOperation,
        operation
    );

    finalOperations += "\n\n"; // FFPE::Rendering::ShaderGen::SG_NEWLINE;
}

void sendData(GLuint program) override {
    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) {
        GLint alphaTestThresholdUniLoc = Uniforms::getCachedUniformLocation(program, "alphaTestThreshold");
        if (alphaTestThresholdUniLoc == -1) return;
        
        glUniform1f(alphaTestThresholdUniLoc, States::AlphaTest::threshold);
    }
}

};

inline AlphaTestFeature alphaTest;
}