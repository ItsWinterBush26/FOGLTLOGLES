#include "es/framebuffer.h"
#include "gl/header.h"
#include "gles30/main.h"
#include "main.h"

void OV_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);

void GLES30::registerTextureOverrides() {
    REGISTEROV(glReadPixels);
}

void OV_glReadPixels(
    GLint x, GLint y,
    GLsizei width, GLsizei height,
    GLenum format, GLenum type, void *pixels
) {
    if (format == GL_DEPTH_COMPONENT) {
        fakeDepthbuffer->data = pixels;
        fakeDepthbuffer->width = width;
        fakeDepthbuffer->height = height;
        fakeDepthbuffer->store(x, y, width, height);
    } else {
        glReadPixels(
            x, y,
            width, height,
            format, type, pixels
        );
    }
}