#pragma once

#include <GLES3/gl3.h>
#include <unordered_map>
#include <vector>

enum SwizzleOperation {
    BGRA2RGBA,
    ENDIANNESS_SWAP,
    ENDIANNESS_SWAP_REV
};

inline const std::unordered_map<SwizzleOperation, std::vector<GLint>> swizzleArrayMap = {
    { BGRA2RGBA,           { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA } },
    { ENDIANNESS_SWAP,     { GL_ALPHA, GL_BLUE, GL_GREEN, GL_RED } },
    { ENDIANNESS_SWAP_REV, { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA } }
};

inline void transformSwizzle(GLint* result, const GLint* input, const GLint* transform) {
    GLint temp[4];
    for (int i = 0; i < 4; i++) {
        switch(transform[i]) {
            case GL_RED:    temp[i] = input[0]; break;
            case GL_GREEN:  temp[i] = input[1]; break;
            case GL_BLUE:   temp[i] = input[2]; break;
            case GL_ALPHA:  temp[i] = input[3]; break;
            default:        temp[i] = transform[i]; break;
        }
    }
    memcpy(result, temp, 4 * sizeof(GLint));
}

inline void doSwizzling(GLenum target, const std::vector<SwizzleOperation>& operations) {
    if (operations.empty()) return;
    
    GLint currentSwizzle[4];
    const GLint* firstSwizzle = swizzleArrayMap.at(operations[0]).data();
    if (!firstSwizzle) return;
    
    memcpy(currentSwizzle, firstSwizzle, 4 * sizeof(GLint));
    
    for (size_t i = 1; i < operations.size(); i++) {
        const GLint* nextTransform = swizzleArrayMap.at(operations[i]).data();
        if (nextTransform) {
            transformSwizzle(currentSwizzle, currentSwizzle, nextTransform);
        }
    }
    
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, currentSwizzle[0]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, currentSwizzle[1]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, currentSwizzle[2]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, currentSwizzle[3]);
}