#pragma once

#include <GLES2/gl2.h>

inline void selectProperIFormat(GLenum& internalFormat) {
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
            internalFormat = GL_DEPTH_COMPONENT16;
            break;
    }
}