#pragma once

#include "es/ffp.h"
#include "es/ffpe/shadergen/alphatest.h"
#include "es/ffpe/shadergen/cache.h"
#include "glm/gtc/type_ptr.hpp"

#include <GLES3/gl32.h>

namespace FFPE::Rendering::ShaderGen::Uniforms {

inline void setupInputsForRendering(GLuint program) {
    glUniformMatrix4fv(
        Cache::Uniforms::getCachedUniformLocation(
            program, "uModelViewProjection"
        ), 1, GL_FALSE,
        glm::value_ptr(
            Matrices::matricesStateManager->getModelViewProjection()
        )
    );
    
    Feature::alphaTestFeatureInstance.sendData(program);
}

}
