#pragma once

#include "utils/types.h"
#include "utils/log.h"

#include <atomic>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

static std::atomic_bool extensionSetInitialized = ATOMIC_VAR_INIT(false);

void initExtensionsES2();
void initExtensionsES3();

namespace ESUtils {
    static std::pair<int, int> version = std::make_pair(0, 0); // major, minor
    static std::unordered_set<str> extensions;
    static int extensionCount;

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