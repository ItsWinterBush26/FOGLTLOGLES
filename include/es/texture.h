#pragma once

#include "es/swizzling.h"
#include <GLES3/gl32.h>
#include <unordered_map>

// texture, internal format
inline std::unordered_map<GLint, GLint> trackedTextures;

inline void trackTextureFormat(GLint& internalFormat) {
    GLint boundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
    trackedTextures.insert({ boundTexture, internalFormat });
}

inline void fixTexArguments(GLenum& target, GLint& internalFormat, GLenum& type, GLenum& format) {
    std::vector<SwizzleOperation> swizzlingOperations { };

    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            type = GL_UNSIGNED_SHORT;
        break;

        case 0x81a7: // GL_DEPTH_COMPONENT32
            internalFormat = GL_DEPTH_COMPONENT24;
        case GL_DEPTH_COMPONENT24:
            type = GL_UNSIGNED_INT;
        break;

        case 35898: // GL_RGB16F
            // if (ESUtils::version.first != 3) LOGW("GL_RGB16F isn't supported below ES 3");
            type = GL_UNSIGNED_BYTE; // GL_FLOAT;
        break;

        case GL_RGBA:
            switch (format) {
                case 0x80e1: // GL_BGRA
                    format = GL_RGBA;

                    switch (type) {
                        case 0x8035: // GL_UNSIGNED_INT_8_8_8_8
                            swizzlingOperations.push_back(ENDIANNESS_SWAP);
                        /*   \/ \/ \/   */
                        case 0x8367: // GL_UNSIGNED_INT_8_8_8_8_REV
                            type = GL_UNSIGNED_BYTE; // this might need to go
                        break;                       // first but idc anymore
                    }

                    swizzlingOperations.push_back(BGRA2RGBA);
                break;
            }
        break;
    }

    doSwizzling(target, swizzlingOperations);
}
