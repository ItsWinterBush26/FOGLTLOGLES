#pragma once

#include "es/ffpe/matrices.h"
#include "es/ffpe/shadergen/cache.h"
#include "es/ffpe/shadergen/features/registry.h"
#include "glm/gtc/type_ptr.hpp"

#include <GLES3/gl32.h>

namespace FFPE::Rendering::ShaderGen::Uniforms {

inline void setupInputsForRendering(GLuint program) {
    glUniformMatrix4fv(
        Cache::Uniforms::getCachedUniformLocation(
            program, "uModelViewProjection"
        ), 1, GL_FALSE,
        glm::value_ptr(
            Matrices::getModelViewProjection()
        )
    );
    
    for (const auto& feature : Feature::Registry::Data::registeredFeatures) {
        feature.sendData(program);
    }
}

}
