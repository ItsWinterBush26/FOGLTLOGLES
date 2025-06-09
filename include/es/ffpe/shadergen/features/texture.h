#pragma once

#include "es/ffpe/shadergen/features/base.h"
#include "es/ffpe/shadergen/features/registry.h"

namespace FFPE::Rendering::ShaderGen::Feature::Texture {

struct TextureFeature : public Feature::BaseFeature {

void buildVS(
    [[maybe_unused]] std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {

}

virtual void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) const override {
    
}


void sendData(GLuint program) const override {
    
}

};

__attribute__((constructor(65535)))
inline void _doRegister() {
    Registry::registerFeature<TextureFeature>();
}

}
