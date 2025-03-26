#pragma once

#include "es/binding_saver.h"

#include <GLES3/gl3.h>

typedef SaveBoundedTexture TextureBindingSaver;

struct BufferBindingSaver {
    GLint savedBuffer;
    GLenum target;

    BufferBindingSaver(GLenum bufferTarget) : target(bufferTarget) {
        glGetIntegerv(getBindingEnum(target), &savedBuffer);
    }

    ~BufferBindingSaver() {
        glBindBuffer(target, savedBuffer);
    }

    static GLenum getBindingEnum(GLenum target) {
        switch (target) {
            case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
            case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
            case GL_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER_BINDING;
            default: return GL_ARRAY_BUFFER_BINDING;
        }
    }
};

struct VAOBindingSaver {
    GLint savedVAO;

    VAOBindingSaver() {
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &savedVAO);
    }

    ~VAOBindingSaver() {
        glBindVertexArray(savedVAO);
    }
};

struct FramebufferBindingSaver {
    GLint savedFBO;

    FramebufferBindingSaver() {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &savedFBO);
    }

    ~FramebufferBindingSaver() {
        glBindFramebuffer(GL_FRAMEBUFFER, savedFBO);
    }
};