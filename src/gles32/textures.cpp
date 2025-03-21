#include "es/proxy.h"
#include "gles32/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params);

// this should be in gles3.1 but idrc
void GLES32::registerTextureOverrides() {
    REGISTEROV(glGetTexLevelParameteriv);
    REGISTEROV(glGetTexLevelParameterfv);
}

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) {
    LOGI("glGetTexLevelParameteriv: target=%u level=%i pname=%u", target, level, pname);
    
    if (isProxyTexture(target)) {
        getProxyTexLevelParameter(pname, params);
        return;
    }

    glGetTexLevelParameteriv(target, level, pname, params);
}

void OV_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params) {
    LOGI("glGetTexLevelParameterfv: target=%u level=%i pname=%u", target, level, pname);
    
    if (isProxyTexture(target)) {
        GLint tmp = 0;
        getProxyTexLevelParameter(pname, &tmp);

        (*params) = static_cast<GLfloat>(tmp);
        return;
    }

    glGetTexLevelParameterfv(target, level, pname, params);
}
