#include "es/framebuffer.h"
#include "gles20/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>

void OV_glGetFramebufferAttachmentParameteriv(GLenum, GLenum, GLenum, GLint*);

void GLES20::registerFramebufferOverride() {
    REGISTEROV(glGetFramebufferAttachmentParameteriv);
}

void OV_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) {
    LOGI("glFrameBufferAttachmentParameteriv: target=%u attachment=%u pname=%u", target, attachment, pname);
    ensureCorrectAttachmentPName(pname);

    glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}