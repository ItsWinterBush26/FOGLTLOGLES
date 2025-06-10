#pragma once

#include "es/ffpe/states.hpp"
#include "es/ffpe/shadergen/cache.hpp"
#include "es/ffpe/shadergen/common.hpp"
#include "es/ffpe/shadergen/features/base.hpp"
#include "es/state_tracking.hpp"
#include "fmt/base.h"
#include "fmt/ostream.h"
#include "gles/ffp/enums.hpp"

#include <GLES3/gl32.h>
#include <sstream>
#include <string>

namespace FFPE::Rendering::ShaderGen::Feature::AlphaTest {

struct AlphaTestFeature : public Feature::BaseFeature {

static constexpr std::string_view uniforms = "uniform float alphaTestThreshold;";
static constexpr std::string_view baseOperation = "if (color.a {} alphaTestThreshold) discard;";

void buildFS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    [[maybe_unused]] std::stringstream& finalOutputOperations
) const override {
    if (!trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) return;

    finalInputs << uniforms << Common::Whitespaces::DOUBLE_NEWLINE;

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

        case GL_NEVER: finalOperations << "discard;"; return;
        case GL_ALWAYS: return;
    }

    fmt::print(
        finalOperations,
        baseOperation,
        operation
    );

    finalOperations << Common::Whitespaces::DOUBLE_NEWLINE_TAB;
}

void sendData(GLuint program) const override {
    if (!trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) return;

    GLint alphaTestThresholdUniLoc = Cache::Uniforms::getCachedUniformLocation(program, "alphaTestThreshold");
    if (alphaTestThresholdUniLoc == -1) return;
        
    glUniform1f(alphaTestThresholdUniLoc, States::AlphaTest::threshold);
}

};

}