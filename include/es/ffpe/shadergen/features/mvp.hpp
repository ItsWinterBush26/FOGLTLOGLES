#pragma once

#include "es/ffpe/matrices.hpp"
#include "es/ffpe/shadergen/cache.hpp"
#include "es/ffpe/shadergen/common.hpp"
#include "es/ffpe/shadergen/features/base.hpp"
#include "glm/gtc/type_ptr.hpp"


#include <GLES3/gl32.h>
#include <sstream>

namespace FFPE::Rendering::ShaderGen::Feature::MVP {

struct MVPFeature : public Feature::BaseFeature {

void buildVS(
    std::stringstream& finalInputs,
    [[maybe_unused]] std::stringstream& finalOutputs,
    [[maybe_unused]] std::stringstream& finalOperations,
    [[maybe_unused]] std::stringstream& finalOutputOperations
) const override {
    finalInputs << "uniform mat4 uModelViewProjection;" << Common::Whitespaces::DOUBLE_NEWLINE;
}

void sendData(GLuint program) const override {
    glUniformMatrix4fv(
        Cache::Uniforms::getCachedUniformLocation(
            program, "uModelViewProjection"
        ), 1, GL_FALSE,
        glm::value_ptr(
            Matrices::getModelViewProjection()
        )
    );
}

};

}
