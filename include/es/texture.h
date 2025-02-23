#pragma once

#include <GLES2/gl2.h>

static inline void selectProperTexType(GLint internalFormat, GLenum& type) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            type = GL_UNSIGNED_SHORT;
            break;
    }
}