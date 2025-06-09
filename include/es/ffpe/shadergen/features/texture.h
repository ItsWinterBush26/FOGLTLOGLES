#pragma once

#include "es/ffpe/shadergen/features/base.h"

namespace FFPE::Rendering::ShaderGen::Feature::Texture {

struct TextureFeature : public Feature::BaseFeature {

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) override {

}

virtual void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) override {
    
}


void sendData(GLuint program) override {
    
}

};

inline TextureFeature instance;

}
