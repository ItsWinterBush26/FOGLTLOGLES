#pragma once

#include "utils/log.h"
#include "utils/types.h"

#include <atomic>
#include <GLES3/gl31.h>
#include <sstream>
#include <unordered_set>

inline std::atomic_bool esUtilsInitialized = ATOMIC_VAR_INIT(false);

void initExtensionsES2();
void initExtensionsES3();

namespace ESUtils {
    inline std::pair<int, int> version = std::make_pair(0, 0); // major, minor
    inline int shadingVersion; // (major * 100) + (minor * 10)
    
    inline std::unordered_set<str> realExtensions;
    inline std::unordered_set<str> fakeExtensions;

    inline bool isAngle = false;
    inline std::tuple<int, int, int> angleVersion = std::make_tuple(0, 0, 0);

    inline void init() {
        if (esUtilsInitialized.load()) return;
        
        str versionStr = reinterpret_cast<str>(glGetString(GL_VERSION));
        if (!versionStr) {
            throw std::runtime_error("Failed to get OpenGL ES version, is the context current or is there no context at all?");
        }

        int major = 0, minor = 0;
        if (sscanf(versionStr, "OpenGL ES %d.%d", &major, &minor) != 2) {
            throw std::runtime_error("Failed to get OpenGL ES version, is the context current or is there no context at all?");
        }
        version = std::make_pair(major, minor);
        shadingVersion = (major * 100) + (minor * 10); // 3 -> 300, 2 -> 20 = 320 = 3.2

        int angleMajor = 0, angleMinor = 0, anglePatch = 0; // ts just made up
        if (sscanf(versionStr, "(ANGLE %d.%d.%d", &angleMajor, &angleMinor, &anglePatch) == 3) {
            isAngle = true;
            angleVersion = std::make_tuple(angleMajor, angleMinor, anglePatch);
        }

        switch (major) {
            case 1:
                throw std::runtime_error("OpenGL ES 1.x is NOT supported");
            case 2:
                initExtensionsES2();
            default:
                initExtensionsES3();
        }

        fakeExtensions = realExtensions;

        esUtilsInitialized.store(true);
    }

    inline bool isExtensionSupported(str name) {
        if (!esUtilsInitialized.load()) {
            LOGW("Extension set wasn't initialized!");
            ESUtils::init();
        }
        return realExtensions.find(name) != realExtensions.end();
    }
}

inline void initExtensionsES2() {
    str extensions = reinterpret_cast<str>(glGetString(GL_EXTENSIONS));
    if (extensions) {
        std::istringstream iss(extensions);
        std::string extension;
        while (iss >> extension) {
            ESUtils::realExtensions.insert(extension.c_str());
        }
    }
}

inline void initExtensionsES3() {
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    for (GLint i = 0; i < extensionCount; ++i) {
        str extension = reinterpret_cast<str>(glGetStringi(GL_EXTENSIONS, i));
        if (extension) ESUtils::realExtensions.insert(extension);
    }
}
