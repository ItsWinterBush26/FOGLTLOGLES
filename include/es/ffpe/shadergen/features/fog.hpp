#pragma once

#include "es/ffpe/matrices.hpp"
#include "es/ffpe/shadergen/cache.hpp"
#include "es/ffpe/shadergen/common.hpp"
#include "es/ffpe/shadergen/features/base.hpp"
#include "es/ffpe/states.hpp"
#include "es/state_tracking.hpp"
#include "gles/ffp/enums.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GLES3/gl32.h>
#include <stdexcept>

namespace FFPE::Rendering::ShaderGen::Feature::Fog {

struct FogFeature : public Feature::BaseFeature {

static constexpr std::string_view depthValueOutputVS = "out float depthValue;";
static constexpr std::string_view modelViewUniformVS = "uniform mat4 uModelView;";

static constexpr std::string_view depthValueInputFS = "in float depthValue;";
static constexpr std::string_view fogDefaultUniformsFS = "uniform vec4 uFogColor;";
static constexpr std::string_view fogLinearUniformsFS = R"(uniform float uFogStart;
uniform float uFogEnd;)";
static constexpr std::string_view fogExpUniformFS = "uniform float uFogDensity;";

bool isEnabled() const override { return trackedStates->isCapabilityEnabled(GL_FOG); }

void buildVS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    [[maybe_unused]] std::stringstream& finalOperations,
    std::stringstream& finalOutputOperations
) const override {
    auto* vertexArray = States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    
    finalInputs << modelViewUniformVS << Common::Whitespaces::DOUBLE_NEWLINE;

    finalOutputs << depthValueOutputVS << Common::Whitespaces::DOUBLE_NEWLINE;

    finalOutputOperations << getDepthValueExpression(
        vertexArray->parameters.size
    ) << Common::Whitespaces::DOUBLE_NEWLINE_TAB;
}

void buildFS(
    std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    std::stringstream& finalOutputOperations
) const override {
    auto* colorArray = States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    
    finalInputs << depthValueInputFS << Common::Whitespaces::SINGLE_NEWLINE;
    finalInputs << fogDefaultUniformsFS << Common::Whitespaces::SINGLE_NEWLINE;
    finalInputs << getFogUniforms() << Common::Whitespaces::DOUBLE_NEWLINE;

    finalOperations << getFogFactorExpression() << Common::Whitespaces::DOUBLE_NEWLINE_TAB;

    finalOutputOperations << getOutputColorExpression(
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    );
}

void sendData(GLuint program) const override {
    glUniformMatrix4fv(
        Cache::Uniforms::getCachedUniformLocation(
            program, "uModelView"
        ), 1, GL_FALSE,
        glm::value_ptr(
            Matrices::getMatrix(GL_MODELVIEW).matrix
        )
    );

    glUniform4fv(
        Cache::Uniforms::getCachedUniformLocation(
            program, "uFogColor"
        ), 1, glm::value_ptr(
            States::Fog::fogColor
        )
    );

    switch (States::Fog::fogMode) {
        case GL_LINEAR:
            glUniform1f(
                Cache::Uniforms::getCachedUniformLocation(
                    program, "uFogStart"
                ), States::Fog::Linear::fogStart
            );

            glUniform1f(
                Cache::Uniforms::getCachedUniformLocation(
                    program, "uFogEnd"
                ), States::Fog::Linear::fogEnd
            );
        break;

        case GL_EXP:
        case GL_EXP2:
            glUniform1f(
                Cache::Uniforms::getCachedUniformLocation(
                    program, "uFogDensity"
                ), States::Fog::Exp::fogDensity
            );
        break;

        default: throw std::runtime_error("Failed to bind fog uniforms! Is Fog::fogMode not GL_LINEAR nor GL_EXP nor GL_EXP2?");
    }
}

std::string getFogUniforms() const {
    switch (States::Fog::fogMode) {
        case GL_LINEAR: return std::string(fogLinearUniformsFS);
        case GL_EXP:
        case GL_EXP2:
            return std::string(fogExpUniformFS);

        default: throw std::runtime_error("Failed to determine fog uniforms! Is Fog::fogMode not GL_LINEAR nor GL_EXP nor GL_EXP2?");
    }
}

std::string getDepthValueExpression(GLint componentSize) const {
    switch (componentSize) {
        case 1: return "depthValue = uModelView * vec4(iVertexPosition, 0, 0, 1);";
        case 2: return "depthValue = uModelView * vec4(iVertexPosition, 0, 1);";
        case 3: return "depthValue = uModelView * vec4(iVertexPosition, 1);";
        case 4: return "depthValue = uModelView * iVertexPosition;";
        default: throw std::runtime_error("Failed to determine position expression! Is componentSize bigger than 4?");
    }
}

std::string getFogFactorExpression() const {
    switch (States::Fog::fogMode) {
        case GL_LINEAR:
            return "float fogFactor = (uFogEnd - depthValue) / (uFogEnd - uFogStart);";
        case GL_EXP:
            return "float fogFactor = exp(-uFogDensity * depthValue);";
        case GL_EXP2:
            return "float fogFactor = exp(-pow(uFogDensity * depthValue, 2.0));";
        default:
            throw std::runtime_error("Failed to determine fog factor expression! Is Fog::fogMode not GL_LINEAR nor GL_EXP nor GL_EXP2?");
    }
}

std::string getOutputColorExpression(GLint componentSize) const {
    switch (componentSize) {
        case 1: return "oFragColor = mix(uFogColor, vec4(color, 0, 0, 1), fogFactor);";
        case 2: return "oFragColor = mix(uFogColor, vec4(color, 0, 1), fogFactor);";
        case 3: return "oFragColor = mix(uFogColor, vec4(color, 1), fogFactor);";
        case 4: return "oFragColor = mix(uFogColor, color, fogFactor);";
        default: throw std::runtime_error("Failed to determine output color expression! Is componentSize bigger than 4?");
    }
}

};

}