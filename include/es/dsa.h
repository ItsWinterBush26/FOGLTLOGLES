#pragma once

#include "es/binding_saver.h"

#include <GLES3/gl3.h>

typedef SaveBoundedTexture TextureBindingSaver;
typedef SaveBoundedBuffer BufferBindingSaver;

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