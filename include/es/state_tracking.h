#pragma once

#include "utils/pointers.h"

#include <GLES3/gl3.h>
#include <memory>
#include <unordered_map>
#include <utility>

struct TextureStates {
    // Type, Texture
    std::unordered_map<GLenum, GLuint> boundTextures;

    // Texture, Internal Format
    std::unordered_map<GLuint, GLenum> textureInternalFormats;
};

struct TrackedStates {
    GLuint activeTextureUnit;

    // Unit, TextureStates
    // Unit is GL_TEXTUREi, where i is 0 to max texture units
    std::unordered_map<GLuint, TextureStates> textureUnits;
    
    GLuint boundReadFramebuffer;
    GLuint boundDrawFramebuffer;

    // Type, Framebuffer
    std::pair<GLenum, GLuint> recentlyBoundFramebuffer; // (either read or draw) (or both? GL_FRAMEBUFFER)

    GLuint boundRenderbuffer;

    // Renderbuffer, Internal Format
    std::unordered_map<GLuint, GLenum> renderbufferInternalFormats;

    // Type, Buffer
    std::unordered_map<GLenum, GLuint> boundBuffers;
};

inline std::shared_ptr<TrackedStates> trackedStates = MakeAggregateShared<TrackedStates>();
