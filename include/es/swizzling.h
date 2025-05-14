#pragma once

#include <GLES3/gl3.h>
#include <unordered_map>
#include <vector>

enum SwizzleOperation {
    BGRA2RGBA,
    ENDIANNESS_SWAP
};

inline const std::unordered_map<SwizzleOperation, std::vector<GLint>> swizzleArrayMap = {
    { BGRA2RGBA,           { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA } },
    { ENDIANNESS_SWAP,     { GL_ALPHA, GL_BLUE, GL_GREEN, GL_RED } }
};

// here to avoid recreating a new vector for each swizzlesZ
// do not forget to resize this to match the
// number of swizzle operations
inline std::vector<SwizzleOperation> currentSwizzleOperations(2);

// hardcoded to 4 channels only
// avoid recreations!
inline std::vector<GLint> transformedComponents(4);

inline std::vector<GLint> transformComponents(
    const std::vector<GLint>& current, 
    const std::vector<GLint>& transform
) {
    for (int i = 0; i < 4; i++) {
        GLint component = transform[i];
        if (component >= GL_RED && component <= GL_ALPHA) {
            // Map through the existing swizzle
            transformedComponents[i] = current[component - GL_RED];
        } else {
            // Pass through non-swizzle values
            transformedComponents[i] = component;
        }
    }

    return transformedComponents;
}

inline void doSwizzling(GLenum target) {
    if (currentSwizzleOperations.empty()) return;
    std::vector<GLint> currentSwizzle = swizzleArrayMap.at(currentSwizzleOperations[0]);
    
    if (currentSwizzleOperations.size() == 1) goto swizzle;
    for (size_t i = 1; i < currentSwizzleOperations.size(); i++) {
        const std::vector<GLint>& nextTransform = swizzleArrayMap.at(currentSwizzleOperations[i]);
        currentSwizzle = transformComponents(currentSwizzle, nextTransform);
    }
    
swizzle:
    // Apply the final swizzle settings to the texture
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, currentSwizzle[0]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, currentSwizzle[1]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, currentSwizzle[2]);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, currentSwizzle[3]);

    currentSwizzleOperations.clear();
}
