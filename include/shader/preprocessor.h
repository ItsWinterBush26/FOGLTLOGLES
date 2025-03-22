#pragma once

#include <glsl-parser/parser.h>
#include <stdexcept>

namespace ShaderConverter::ASTPreprocessor {
    inline void preprocessGLSL(int kind, std::string& source) {
        glsl::parser parser(source);
        glsl::astTU* translationUnit = parser.parse(kind);

        if (!translationUnit) {
            LOGE("Failed to preprocess the GLSL shader! %s", parser.error());
            throw std::runtime_error("Failed to preprocess GLSL shader!");
        }
    }
}