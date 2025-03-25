#pragma once

#include <GLES2/gl2.h>
#include <unordered_map>

inline std::unordered_map<GLenum, GLint> trackedRenderbuffers;

inline void trackRenderbufferFormat(GLenum& internalFormat) {
    GLint boundRenderbuffer;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRenderbuffer);
    trackedRenderbuffers.insert({ boundRenderbuffer, internalFormat });
}

inline void fixStorageParams(GLenum& internalFormat) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            internalFormat = GL_DEPTH_COMPONENT16;
            break;
    }
}
