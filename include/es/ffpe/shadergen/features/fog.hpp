#pragma once

#include "es/ffpe/shadergen/features/base.hpp"

namespace FFPE::Rendering::ShaderGen::Feature::Fog {

struct FogFeature : public Feature::BaseFeature {

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    [[maybe_unused]] std::stringstream& finalOutputOperations
) const override {

}

void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations,
    [[maybe_unused]] std::stringstream& finalOutputOperations
) const override {
    
}

void sendData(GLuint program) const override {
    
}

};

}