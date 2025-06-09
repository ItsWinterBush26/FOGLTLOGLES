#pragma once

#include "es/ffpe/matrices.hpp"
#include "es/ffpe/shadergen/features/registry.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GLES3/gl32.h>

namespace FFPE::Rendering::ShaderGen::Uniforms {

inline void setupInputsForRendering(GLuint program) {
    for (const auto* feature : Feature::Registry::Data::registeredFeatures) {
        feature->sendData(program);
    }
}

}
