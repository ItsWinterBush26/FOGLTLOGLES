#pragma once

#include "es/ffp.h"
#include "es/ffpe/shadergen/cache.h"
#include "es/ffpe/shadergen/common.h"
#include "es/ffpe/shadergen/features/base.h"
#include "fmt/base.h"
#include "fmt/ostream.h"

#include <GLES3/gl32.h>
#include <sstream>
#include <string>

namespace FFPE::Rendering::ShaderGen::Feature::AlphaTest {

struct AlphaTestFeature : public Feature::BaseFeature {

const std::string uniforms = "uniform float alphaTestThreshold;";

const std::string baseOperation = "if (color.a {} alphaTestThreshold) discard;";

void buildVS(
    std::stringstream&, std::stringstream&, std::stringstream&
) override { }

void buildFS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) override {
    finalInputs << uniforms << Common::SG_NEWLINE;

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
        fmt::runtime(baseOperation),
        operation
    );

    finalOperations << Common::SG_NEWLINE;
}

void sendData(GLuint program) override {
    GLint alphaTestThresholdUniLoc = Cache::Uniforms::getCachedUniformLocation(program, "alphaTestThreshold");
    if (alphaTestThresholdUniLoc == -1) return;
        
    glUniform1f(alphaTestThresholdUniLoc, States::AlphaTest::threshold);
}

};

inline AlphaTestFeature instance;

}