#pragma once

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::ShaderGen::Feature {

struct BaseFeature {

virtual ~BaseFeature() = default;

virtual void build(
    std::string& finalInputs,
    std::string& finalOutputs,
    std::string& finalOperations
);
virtual void sendData(GLuint program);

};

}