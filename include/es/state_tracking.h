#pragma once

#include "utils/log.h"
#include "utils/pointers.h"
#include "utils/fast_map.h"

#include <GLES3/gl3.h>
#include <memory>
#include <unordered_map>
#include <utility>

#ifndef MAX_FBTARGETS
#define MAX_FBTARGETS 8
#endif

#ifndef MAX_DRAWBUFFERS
#define MAX_DRAWBUFFERS 8
#endif

struct Framebuffer;

struct TextureStates {
    std::pair<GLenum, GLuint> recentlyBoundTexture;

    // Type, Texture
    FAST_MAP_BI(GLenum, GLuint) boundTextures;

    // Texture, Internal Format
    FAST_MAP_BI(GLuint, GLenum) textureInternalFormats;

    void bindTextureToTarget(GLenum target, GLuint texture) {
        boundTextures[target] = texture;
        recentlyBoundTexture = std::make_pair(target, texture);
    }

    GLenum getTextureInternalFormat(GLenum target) {
        return textureInternalFormats[boundTextures[target]];
    }
};

struct FramebufferStates {
    GLuint boundReadFramebuffer;
    GLuint boundDrawFramebuffer;

    // Type, Framebuffer
    std::pair<GLenum, GLuint> recentlyBoundFramebuffer; // (either read or draw) (or both? GL_FRAMEBUFFER)

    // Framebuffer, Framebuffer struct
    //     | |
    // since we are using the framebuffer object
    // we need to make sure it gets erased here
    FAST_MAP_BI(GLuint, std::shared_ptr<Framebuffer>) trackedFramebuffers;

};

struct TrackedStates {
    GLuint activeTextureUnit = GL_TEXTURE0;
    TextureStates* activeTextureState;

    // Unit, TextureStates
    // Unit is GL_TEXTUREi, where i is 0 to max texture units
    FAST_MAP_BI(GLuint, TextureStates) textureUnits;

    FramebufferStates framebufferState;
    
    GLuint boundRenderbuffer;

    // Renderbuffer, Internal Format
    FAST_MAP_BI(GLuint, GLenum) renderbufferInternalFormats;

    // Type, Buffer
    FAST_MAP_BI(GLenum, GLuint) boundBuffers;

    TrackedStates() {
        GLint maxTextureUnits;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
        textureUnits.reserve(maxTextureUnits);
        LOGI("Reserving %i texture units", maxTextureUnits);

        activeTextureState = &textureUnits[GL_TEXTURE0];
    }

    void setActiveTextureUnit(GLuint unit) {
        activeTextureUnit = unit;
        activeTextureState = &textureUnits[unit];
    }
};

inline std::shared_ptr<TrackedStates> trackedStates;

struct FramebufferColorInfo {
    GLuint colorTargets[MAX_FBTARGETS];
    GLuint colorObjects[MAX_FBTARGETS];
    GLuint colorComponentType[MAX_FBTARGETS];
    GLuint colorEncoding[MAX_FBTARGETS];

    // if GL_TEXTURE
    GLuint colorLevels[MAX_FBTARGETS];
    GLuint colorLayers[MAX_FBTARGETS];
};

struct Framebuffer {
    FramebufferColorInfo colorInfo;
    GLenum virtualDrawbuffers[MAX_DRAWBUFFERS] = { GL_COLOR_ATTACHMENT0 };
    GLenum physicalDrawbuffers[MAX_DRAWBUFFERS];
    GLsizei bufferAmount = 1;
};