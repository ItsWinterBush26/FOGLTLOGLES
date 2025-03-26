#pragma once

#include "es/texture.h"

#include <GLES3/gl32.h>

struct SaveBoundedTexture {
    GLint boundedTexture;
    GLint activeTexture;
    GLenum textureType;

    SaveBoundedTexture(GLenum textureType) : textureType(textureType) {
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
        glGetIntegerv(getEnumBindingForTarget(textureType), &boundedTexture);
    }

    ~SaveBoundedTexture() {
        glActiveTexture(activeTexture);
        glBindTexture(textureType, boundedTexture);
    }
};
