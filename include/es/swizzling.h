#pragma once

#include <GLES3/gl3.h>

enum SwizzleOperation {
    BGRA2RGBA
};

inline const GLint swizzleBGRA[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };

inline void doSwizzling(SwizzleOperation operation, GLenum target) {
    switch (operation) {
        case BGRA2RGBA:
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, swizzleBGRA[0]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, swizzleBGRA[1]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, swizzleBGRA[2]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, swizzleBGRA[3]);
            break;
    }
}