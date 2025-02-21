#include "gles20/translation.h"
#include "main.h"
#include "gles20/proxy.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void glClearDepth(double d);
void OV_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length),

static GLint maxTextureSize = 0;

void GLES20::registerTranslatedFunctions() {
    LOGI("Hi from translation.cpp!");
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    REGISTER(glClearDepth);
    REGISTEROV(glTexImage2D);
    REGISTEROV(glGetTexLevelParameteriv);
}

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

GLint proxyWidth, proxyHeight, proxyInternalFormat;

void OV_glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width, GLsizei height,
    GLint border, GLenum format,
    GLenum type, const void* pixels
) {
    if (isProxyTexture(target)) {
        proxyWidth = (( width << level ) > maxTextureSize) ? 0 : width;
        proxyHeight = (( height << level ) > maxTextureSize) ? 0 : height;
        proxyInternalFormat = internalFormat;

        return;
    }

    glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
}

void OV_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) {
    if (isProxyTexture(target)) {
        switch (pname) {
            case GL_TEXTURE_WIDTH:
                (*params) = nlevel(proxyWidth, level);
                return;
            case GL_TEXTURE_HEIGHT:
                (*params) = nlevel(proxyHeight, level);
                return;
            case GL_TEXTURE_INTERNAL_FORMAT:
                (*params) = proxyInternalFormat;
                return;
        }
    } else {
        glGetTexLevelParameteriv(target,level, pname, params);
    }
}

void OV_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length) {
    
}