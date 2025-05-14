#pragma once

#include <GLES3/gl3.h>
#include <unordered_map>
#include <vector>

enum SwizzleOperation {
    BGRA2RGBA,
    ENDIANNESS_SWAP,
};

inline const std::unordered_map<SwizzleOperation, std::vector<GLint>> swizzleArrayMap = {
    { BGRA2RGBA,           { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA } },
    { ENDIANNESS_SWAP,     { GL_ALPHA, GL_BLUE, GL_GREEN, GL_RED } }
};

inline std::vector<GLint> transformSwizzle(
    const std::vector<GLint>& current, 
    const std::vector<GLint>& transform
) {
    std::vector<GLint> result(4);
    for (int i = 0; i < 4; i++) {
        GLint component = transform[i];
        if (component >= GL_RED && component <= GL_ALPHA) {
            // Map through the existing swizzle
            result[i] = current[component - GL_RED];
        } else {
            // Pass through non-swizzle values
            result[i] = component;
        }
    }
    return result;
}

inline void doSwizzling(GLenum target, const std::vector<SwizzleOperation>& operations) {
    if (operations.empty()) return;
    
    std::vector<GLint> currentSwizzle = swizzleArrayMap.at(operations[0]);
    
    if (operations.size() == 1) goto swizzle;

    for (size_t i = 1; i < operations.size(); i++) {
        const std::vector<GLint>& nextTransform = swizzleArrayMap.at(operations[i]);
        currentSwizzle = transformSwizzle(currentSwizzle, nextTransform);
    }
    
swizzle:
    // Apply the final swizzle settings to the texture
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, currentSwizzle[0]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, currentSwizzle[1]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, currentSwizzle[2]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, currentSwizzle[3]);
}
