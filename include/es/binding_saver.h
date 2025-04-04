#pragma once

#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "gles20/framebuffer_tracking.h"
#include "gles20/texture_tracking.h"

#include <GLES3/gl32.h>

// TODO:
// What if we keep track of bounded textures, buffer, etc ourselves
// as we can intercept calls and because glGets are expensive

// PROGRESS: PARTIAL

struct SaveActiveTextureUnit {
    GLuint activeTextureUnit;

    SaveActiveTextureUnit() {
        activeTextureUnit = trackedStates->activeTextureUnit;
    }

    ~SaveActiveTextureUnit() {
        OV_glActiveTexture(activeTextureUnit);
    }
};

struct SaveBoundedTexture {
    GLuint boundedTexture;
    GLenum textureType;

    SaveBoundedTexture(GLenum textureType) : textureType(textureType) {
        boundedTexture = trackedStates->activeTextureState->boundTextures[textureType];
    }

    ~SaveBoundedTexture() {
        OV_glBindTexture(textureType, boundedTexture);
    }
};

struct SaveBoundedBuffer {
    GLuint boundedBuffer;
    GLenum bufferType;
    
    SaveBoundedBuffer(GLenum bufferType) : bufferType(bufferType) {
        boundedBuffer = trackedStates->boundBuffers[bufferType].buffer;
    }

    ~SaveBoundedBuffer() {
        OV_glBindBuffer(bufferType, boundedBuffer);
    }
};

struct SaveBoundedFramebuffer {
    GLenum framebufferType;
    GLuint boundedFramebuffer;

    SaveBoundedFramebuffer(GLenum framebufferType) : framebufferType(framebufferType) {
        switch (framebufferType) {
            case GL_FRAMEBUFFER:
            case GL_DRAW_FRAMEBUFFER:
                boundedFramebuffer = trackedStates->framebufferState.boundDrawFramebuffer;
            break;

            case GL_READ_FRAMEBUFFER:
                boundedFramebuffer = trackedStates->framebufferState.boundReadFramebuffer;
            break;
        }
    }

    ~SaveBoundedFramebuffer() {
        OV_glBindFramebuffer(framebufferType, boundedFramebuffer);
    }
};
