#pragma once

#include "es/ffpe/matrices.h"
#include "es/ffpe/shadergen/cache.h"
#include "es/ffpe/shadergen/features/alphatest.h"
#include "es/ffpe/shadergen/features/texture.h"
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
    
    if (trackedStates->isCapabilityEnabled(GL_ALPHA_TEST)) 
        Feature::AlphaTest::instance.sendData(program);

    if (trackedStates->isCapabilityEnabled(GL_TEXTURE_2D)) 
        Feature::Textures::instance.sendData(program);

}

}
