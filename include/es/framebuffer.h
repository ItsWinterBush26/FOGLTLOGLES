#pragma once

#include <GLES2/gl2.h>

inline GLuint getAttachmentIndex(GLenum attachment) {
    switch (attachment) {
        case GL_NONE:
        case GL_DEPTH_ATTACHMENT:
        case GL_STENCIL_ATTACHMENT:
        case 0x821a: // GL_DEPTH_STENCIL_ATTACHMENT
            return -1;
    }

    return attachment - 0x8ce0; /* GL_COLOR_ATTACHMENT0 */
}

// inline GLenum getMapAttachment();