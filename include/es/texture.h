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

inline void selectProperTexType(GLenum& target, GLint& internalFormat, GLenum& type) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            type = GL_UNSIGNED_SHORT;
            break;

        case GL_DEPTH_COMPONENT24:
            type = GL_UNSIGNED_INT;
            break;
        
        case 35898: // GL_RGB16F
            // if (ESUtils::version.first != 3) LOGW("GL_RGB16F isn't supported below ES 3");
            type = GL_UNSIGNED_BYTE; // GL_FLOAT;
            break;
        
        case GL_RGBA:
            switch (type) {
                case 0x8367: // GL_UNSIGNED_INT_8_8_8_8_REV
                    type = GL_UNSIGNED_BYTE;
                    doSwizzling(ENDIANNESS_SWAP_REV, target);
                    break;
            }
            break;
    }
}

inline void selectProperTexFormat(GLenum& target, GLint& internalFormat, GLenum& format) {
    switch (internalFormat) {
        case 5898: // GL_RGB16F
            format = GL_RGB;
            break;
        case GL_RGBA:
            switch (format) {
                case 0x80e1: // GL_BGRA
                    format = GL_RGBA;
                    doSwizzling(BGRA2RGBA, target);
                    break;
            }
            break;
    }
}

inline void selectProperTexIFormat(GLint& internalFormat) {
    switch (internalFormat) {
        case 0x81a7: // GL_DEPTH_COMPONENT32
            internalFormat = GL_DEPTH_COMPONENT24;
            break;
    }
}

