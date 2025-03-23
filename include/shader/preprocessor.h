#pragma once

#include "utils/env.h"
#include "utils/log.h"

// #include <glsl-parser/converter.h>
// #include <glsl-parser/parser.h>
#include <stdexcept>

namespace ShaderConverter::ASTPreprocessor {
    inline void preprocessGLSL(int kind, std::string& source) {
        /* glsl::parser parser(source.c_str(), "glsl");
        glsl::astTU* translationUnit = parser.parse(kind);

        if (!translationUnit) {
            LOGE("Failed to preprocess the GLSL shader! %s", parser.error());
            throw std::runtime_error("Failed to preprocess GLSL shader!");
        }

        for (const auto& var : translationUnit->globals) {
            if (var->storage == glsl::kUniform) {
                var->initialValue = nullptr;
            }
        }

        source = std::string(glsl::converter().convertTU(translationUnit));

        if (getEnvironmentVar("LIBGL_VGPU_DUMP") == "1") {
            LOGI("Pre-processed GLSL shader:");
            LOGI("%s", source.c_str());
        } */
    }
}