#pragma once

#include "es/ffpe/shadergen/cache.hpp"
#include "es/ffpe/shadergen/features/registry.hpp"

#include <GLES3/gl32.h>

namespace FFPE::Rendering::ShaderGen::Uniforms {

inline void setupInputsForRendering(GLuint program) {
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Bind uniforms");

    for (const auto* feature : Feature::Registry::Data::registeredFeatures) {
        feature->sendData(program);
    }

    glUniform1i(
        Cache::Uniforms::getCachedUniformLocation(
            program, "tex0"
        ), 0
    );

    glPopDebugGroup();
}

}
