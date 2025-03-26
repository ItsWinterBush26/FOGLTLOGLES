#pragma once

#include <GLES3/gl32.h>

inline GLenum getBindingEnumOfTexture(GLenum& target) {
    switch (target) {
        case GL_TEXTURE_2D: return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
        case GL_TEXTURE_3D: return GL_TEXTURE_BINDING_3D;
        case GL_TEXTURE_2D_ARRAY: return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP: return GL_TEXTURE_BINDING_CUBE_MAP;
        case GL_TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
        case GL_TEXTURE_BUFFER: return GL_TEXTURE_BUFFER_BINDING;
        default: return 0;
    }
}

inline GLenum getBindingEnumOfBuffer(GLenum target) {
    switch (target) {
        case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER_BINDING;
        default: return GL_ARRAY_BUFFER_BINDING;
    }
}

// TODO:
// What if we keep track of bounded textures, buffer, etc ourselves
// as we can intercept calls and glGets are expensive

struct SaveBoundedTexture {
    GLint boundedTexture;
    GLint activeTexture;
    GLenum textureType;

    SaveBoundedTexture(GLenum textureType) : textureType(textureType) {
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
        glGetIntegerv(getBindingEnumOfTexture(textureType), &boundedTexture);
    }

    ~SaveBoundedTexture() {
        glActiveTexture(activeTexture);
        glBindTexture(textureType, boundedTexture);
    }
};

struct SaveBoundedBuffer {
    GLint boundedBuffer;
    GLenum bufferType;
    
    SaveBoundedBuffer(GLenum bufferType) : bufferType(bufferType) {
        glGetIntegerv(getBindingEnumOfBuffer(bufferType), &boundedBuffer);
    }

    ~SaveBoundedBuffer() {
        glBindBuffer(bufferType, boundedBuffer);
    }
};