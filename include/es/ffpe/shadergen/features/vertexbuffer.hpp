#pragma once

#include "es/ffpe/shadergen/features/base.hpp"

#include <GLES3/gl32.h>
#include <sstream>

namespace FFPE::Rendering::ShaderGen::Feature::VertexBuffer {

struct VertexBufferFeature : public Feature::BaseFeature {

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {
    finalOperations << "gl_Position = uModelViewProjection * vec4(iVertexPosition, 1);"

}

virtual void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {
    
}


void sendData(GLuint program) const override {

};

}
