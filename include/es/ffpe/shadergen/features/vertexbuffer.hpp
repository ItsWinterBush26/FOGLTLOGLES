#pragma once

#include "es/ffp.hpp"
#include "es/ffpe/shadergen/common.hpp"
#include "es/ffpe/shadergen/features/base.hpp"
#include "fmt/base.h"
#include "fmt/format.h"

#include <GLES3/gl32.h>
#include <sstream>
#include <stdexcept>
#include <string>

namespace FFPE::Rendering::ShaderGen::Feature::VertexAttrib {

struct VertexAttribFeature : public Feature::BaseFeature {

const std::string vertexPositionInputVS = "layout(location = 0) in mediump vec{} iVertexPosition;";
const std::string vertexColorInputVS = "layout(location = 1) in lowp vec{} iVertexColor;";
const std::string vertexTexCoordInputVS = "layout(location = 2) in mediump vec{} iVertexTexCoord;";
const std::string vertexColorOutputVS = "out lowp vec{} vertexColor;";
const std::string vertexTexCoordOutputVS = "out mediump vec{} vertexTexCoord;";

const std::string vertexColorInputFS = "in lowp vec{} vertexColor;";
const std::string vertexTexCoordInputFS = "in mediump vec{} vertexTexCoord;";
const std::string fragColorOutputFS = "out vec4 oFragColor;";

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    std::stringstream& finalOutputOperations
) const override {
    auto* vertexArray = States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    auto* colorArray = States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    auto* texCoordArray = States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0);

    finalInputs << fmt::format(
        fmt::runtime(vertexPositionInputVS),
        vertexArray->parameters.size
    ) << Common::SG_STMT_NEWLINE;

    finalInputs << fmt::format(
        fmt::runtime(vertexColorInputVS),
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    ) << Common::SG_STMT_NEWLINE;

    finalInputs << fmt::format(
        fmt::runtime(vertexTexCoordInputVS),
        texCoordArray->enabled ? texCoordArray->parameters.size : decltype(States::VertexData::texCoord)::length()
    ) << Common::SG_VAR_NEWLINE;

    finalOutputs << fmt::format(
        fmt::runtime(vertexColorOutputVS),
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    ) << Common::SG_STMT_NEWLINE;

    finalOutputs << fmt::format(
        fmt::runtime(vertexTexCoordOutputVS),
        texCoordArray->enabled ? texCoordArray->parameters.size : decltype(States::VertexData::texCoord)::length()
    ) << Common::SG_VAR_NEWLINE;

    finalOperations << getPositionExpression(vertexArray->parameters.size) << Common::SG_VAR_NEWLINE;

    finalOutputOperations << "vertexColor = iVertexColor;" << Common::SG_STMT_NEWLINE;
    finalOutputOperations << "vertexTexCoord = iVertexTexCoord;" << Common::SG_VAR_NEWLINE;
}

void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    std::stringstream& finalOutputOperations
) const override {
    auto* colorArray = States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    auto* texCoordArray = States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0);

    finalInputs << fmt::format(
        fmt::runtime(vertexColorInputFS),
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    ) << Common::SG_STMT_NEWLINE;

    finalInputs << fmt::format(
        fmt::runtime(vertexTexCoordInputFS),
        texCoordArray->enabled ? texCoordArray->parameters.size : decltype(States::VertexData::texCoord)::length()
    ) << Common::SG_VAR_NEWLINE;

    finalOutputs << fragColorOutputFS << Common::SG_VAR_NEWLINE;

    finalOperations << fmt::format(
        fmt::runtime("lowp vec{} color = vertexColor;"),
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    ) << Common::SG_VAR_NEWLINE;

    finalOutputOperations << getOutputColorExpression(
        colorArray->enabled ? colorArray->parameters.size : decltype(States::VertexData::color)::length()
    ) << Common::SG_STMT_NEWLINE;
}

std::string getPositionExpression(GLint componentSize) const {
    switch (componentSize) {
        case 1: return "gl_Position = uModelViewProjection * vec4(iVertexPosition, 0, 0, 1);";
        case 2: return "gl_Position = uModelViewProjection * vec4(iVertexPosition, 0, 1);";
        case 3: return "gl_Position = uModelViewProjection * vec4(iVertexPosition, 1);";
        case 4: return "gl_Position = uModelViewProjection * iVertexPosition;";
        default: throw std::runtime_error("Failed to determine position expression! Is componentSize bigger than 4?");
    }
}

std::string getOutputColorExpression(GLint componentSize) const {
    switch (componentSize) {
        case 1: return "oFragColor = vec4(color, 0, 0, 1);";
        case 2: return "oFragColor = vec4(color, 0, 1);";
        case 3: return "oFragColor = vec4(color, 1);";
        case 4: return "oFragColor = color;";
        default: throw std::runtime_error("Failed to determine output color expression! Is componentSize bigger than 4?");
    }
}

};

}
