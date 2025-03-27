#pragma once

#include "utils/pointers.h"

#include <GLES3/gl3.h>
#include <memory>
#include <unordered_map>
#include <utility>

struct TrackedStates {
    GLuint activeTexture;
    
    GLuint boundReadFramebuffer;
    GLuint boundDrawFramebuffer;

    // Type, Framebuffer
    std::pair<GLenum, GLuint> recentlyBoundFramebuffer; // (either read or draw) (or both? GL_FRAMEBUFFER)

    // Type, Texture
    std::unordered_map<GLenum, GLuint> boundTextures;

    // Texture, Internal Format
    std::unordered_map<GLuint, GLenum> textureInternalFormats;

    GLuint boundRenderbuffer;

    // Renderbuffer, Internal Format
    std::unordered_map<GLuint, GLenum> renderbufferInternalFormats;

    // Type, Buffer
    std::unordered_map<GLenum, GLuint> boundBuffers;
};

inline std::shared_ptr<TrackedStates> trackedStates = MakeAggregateShared<TrackedStates>();