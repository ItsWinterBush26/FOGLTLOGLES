#pragma once

#include "es/ffp.h"
#include "es/ffpe/shadergen/shadergen.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/fast_map.h"

#include <GLES3/gl32.h>
#include <string>
#include <unordered_map>

namespace FFPE::Rendering::ShaderGen::Uniforms {

namespace Cache {
    inline FastMapBI<GLuint, std::unordered_map<std::string, GLint>> locations;
}

inline GLint getCachedUniformLocation(GLuint program, std::string name) {
    auto& cachedUniformLocations = Cache::locations[program];

    GLint uniformLocationLoc = 0;
    auto uniformLocationIT = cachedUniformLocations.find(name);
    if (uniformLocationIT == cachedUniformLocations.end()) {
        uniformLocationLoc = glGetUniformLocation(program, name.c_str());
        if (uniformLocationLoc != -1) cachedUniformLocations[name] = uniformLocationLoc;
    } else uniformLocationLoc = uniformLocationIT->second;

    return uniformLocationLoc;
}

inline void setupUniformsForRendering(GLuint program) {
    glUniformMatrix4fv(
        getCachedUniformLocation(
            program, "uModelViewProjection"
        ), 1, GL_FALSE,
        glm::value_ptr(
            Matrices::matricesStateManager->getModelViewProjection()
        )
    );

    ShaderGen::prepareInputsForRendering(program);
}

}
