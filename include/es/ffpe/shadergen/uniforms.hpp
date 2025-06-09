#pragma once

#include "es/ffpe/shadergen/features/registry.hpp"

#include <GLES3/gl32.h>

namespace FFPE::Rendering::ShaderGen::Uniforms {

inline void setupInputsForRendering(GLuint program) {
    for (const auto* feature : Feature::Registry::Data::registeredFeatures) {
        feature->sendData(program);
    }
}

}
