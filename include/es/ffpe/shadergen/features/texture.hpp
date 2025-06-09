#pragma once

#include "es/ffpe/shadergen/features/base.hpp"
#include "es/ffpe/shadergen/features/registry.hpp"

namespace FFPE::Rendering::ShaderGen::Feature::Texture {

struct TextureFeature : public Feature::BaseFeature {

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {

}

void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {
    
}

void sendData(GLuint program) const override {
    
}

};

}
