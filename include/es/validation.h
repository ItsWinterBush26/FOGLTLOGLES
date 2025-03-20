#pragma once

#include <GLES2/gl2.h>

namespace Validation {
    inline bool isCurrentFramebufferValid() {
        GLint framebuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    
        // If no framebuffer is bound (0 = default framebuffer)
        if (framebuffer == 0) return true; // The default framebuffer is always valid

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        return (status == GL_FRAMEBUFFER_COMPLETE);
    }    
}