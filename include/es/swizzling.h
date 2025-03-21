#pragma once

#include <GLES3/gl3.h>

enum SwizzleOperation {
    BGRA2RGBA,
    ENDIANNESS_SWAP,
    ENDIANNESS_SWAP_REV
};

inline constexpr GLint swizzleBGRA[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
inline constexpr GLint swizzleEndianSwap[] = { GL_ALPHA, GL_BLUE, GL_GREEN, GL_RED };
inline constexpr GLint swizzleEndianSwapRev[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };

inline void doSwizzling(SwizzleOperation operation, GLenum target) {
    const GLint* swizzle = nullptr;

    switch (operation) {
        case SwizzleOperation::BGRA2RGBA:
            swizzle = swizzleBGRA;
            break;
        case SwizzleOperation::ENDIANNESS_SWAP:
            swizzle = swizzleEndianSwap;
            break;
        case SwizzleOperation::ENDIANNESS_SWAP_REV:
            swizzle = swizzleEndianSwapRev;
            break;
    }

    if (swizzle) {
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, swizzle[0]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, swizzle[1]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, swizzle[2]);
        glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, swizzle[3]);
    }
}