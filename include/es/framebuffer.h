#pragma once

#include <GLES2/gl2.h>

inline void ensureCorrectAttachmentPName(GLenum& pname) {
    switch (pname) {
        case 0x8DA7: // GL_FRAMEBUFFER_ATTACHMENT_LAYERED in gl/glext.h
            pname = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE;
            break;
        default:
            break;
    }
}