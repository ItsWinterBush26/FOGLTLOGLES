#pragma once

#include "shaderc/shaderc.hpp"
#include "utils/types.h"
#include "utils/log.h"

#include <atomic>
#include <GLES3/gl31.h>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <unordered_set>

static std::atomic_bool extensionSetInitialized = ATOMIC_VAR_INIT(false);

void initExtensionsES2();
void initExtensionsES3();

namespace ESUtils {
    static std::pair<int, int> version = std::make_pair(0, 0); // major, minor
    
    static std::unordered_set<str> extensions;
    static int extensionCount;

    static bool isAngle = false;
    static std::tuple<int, int, int> angleVersion = std::make_tuple(0, 0, 0);

    static void init() {
        if (extensionSetInitialized.load()) return;
        
        str versionStr = reinterpret_cast<str>(glGetString(GL_VERSION));
        if (!versionStr) {
            throw std::runtime_error("Failed to get OpenGL ES version, is the context not initialized or what?");
        }

        int major = 0, minor = 0;
        if (sscanf(versionStr, "OpenGL ES %d.%d", &major, &minor) == 2) {
            version = std::make_pair(major, minor);
        }

        int angleMajor = 0, angleMinor = 0, anglePatch = 0; // ts just made up
        if (sscanf(versionStr, "(ANGLE %d.%d.%d", &angleMajor, &angleMinor, &anglePatch) == 3) {
            isAngle = true;
            angleVersion = std::make_tuple(angleMajor, angleMinor, anglePatch);
        }

        LOGI("%s", versionStr);

        switch (major) {
            case 1:
                throw std::runtime_error("OpenGL ES 1.x is NOT supported");
            case 2:
                initExtensionsES2();
            default:
                initExtensionsES3();
        }

        extensionSetInitialized.store(true);
    }

    static inline bool isSupported(str name) {
        if (!extensionSetInitialized.load()) {
            /* throw std::runtime_error */
            LOGW("Extension set wasn't initialized!");
            ESUtils::init();
        }
        return extensions.find(name) != extensions.end();
    }

    static inline shaderc_shader_kind getKindFromShader(GLuint& shader) {
        GLint shaderType;
        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        
        switch (shaderType) {
            case GL_FRAGMENT_SHADER:
                return shaderc_fragment_shader;
            case GL_VERTEX_SHADER:
                return shaderc_vertex_shader;
            case GL_COMPUTE_SHADER:
                if (version.first == 3 && version.second >= 1) return shaderc_compute_shader;
                throw std::runtime_error("You need OpenGL ES 3.1 or newer for compute shaders!");
            
            default:
                LOGI("%u", shader);
                throw std::runtime_error("Received an unsupported shader type!");

        }
    }
}

inline void initExtensionsES2() {
    str extensions = reinterpret_cast<str>(glGetString(GL_EXTENSIONS));
    if (extensions) {
        std::istringstream iss(extensions);
        std::string extension;
        while (iss >> extension) {
            ESUtils::extensions.insert(extension.c_str());
        }
    }

    ESUtils::extensionCount = ESUtils::extensions.size();
}

inline void initExtensionsES3() {
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    for (GLint i = 0; i < extensionCount; ++i) {
        str extension = reinterpret_cast<str>(glGetStringi(GL_EXTENSIONS, i));
        if (extension) ESUtils::extensions.insert(extension);
    }

    ESUtils::extensionCount = extensionCount;
}