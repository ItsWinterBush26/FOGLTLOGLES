#pragma once

#include <GLES3/gl32.h>
#include <sstream>

namespace FFPE::Rendering::ShaderGen::Feature {

struct BaseFeature {

virtual ~BaseFeature() = default;

virtual void buildVS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) = 0;

virtual void buildFS(
    std::stringstream& finalInputs,
    std::stringstream& finalOutputs,
    std::stringstream& finalOperations
) = 0;

virtual void sendData(GLuint program) = 0;

};

}