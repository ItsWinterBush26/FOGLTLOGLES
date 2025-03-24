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

        case GL_R8:
            switch (format) {
                case 0x80e1: // GL_BGRA
                    format = GL_RED;
                    switch (type) {
                        case 0x8035: // GL_UNSIGNED_INT_8_8_8_8
                            swizzlingOperations.push_back(BGRA2RGBA);
                            swizzlingOperations.push_back(ENDIANNESS_SWAP);
                        case 0x8367: // GL_UNSIGNED_INT_8_8_8_8_REV
                            type = GL_UNSIGNED_BYTE; // /\ ts is basically BGRA2RGBA
                        break;
                    }
                break;
            }

        case GL_RGBA:
        case GL_RGBA8:
            switch (format) {
                case 0x80e1: // GL_BGRA
                    format = GL_RGBA;
                    switch (type) {
                        case 0x8035: // GL_UNSIGNED_INT_8_8_8_8
                            swizzlingOperations.push_back(BGRA2RGBA);
                            swizzlingOperations.push_back(ENDIANNESS_SWAP);
                        case 0x8367: // GL_UNSIGNED_INT_8_8_8_8_REV
                            type = GL_UNSIGNED_BYTE; // /\ ts is basically BGRA2RGBA
                        break;
                    }
                break;
            }
        break;

        case 0x8c3a: // GL_R11F_G11F_B10F
            format = GL_RGB;
            type = GL_FLOAT;
        break;

        case GL_RGB16F:
            format = GL_RGB;
            type = GL_HALF_FLOAT;
        break;

        case GL_R16F:
            format = GL_RED;
            type = GL_HALF_FLOAT;
        break;

        case GL_R32F:
            format = GL_RED;
            type = GL_FLOAT;
        break;

        /* case GL_RGBA8:
            type = GL_UNSIGNED_BYTE;
        break; */
    }

    doSwizzling(target, swizzlingOperations);
}
