#include "es/ffp.h"
#include "gles/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void OV_glBindTexture(GLenum target, GLuint texture);

void GLES::registerTexturesOverride() {
    REGISTEROV(glBindTexture);
}

void OV_glBindTexture(GLenum target, GLuint texture) {
    Lists::displayListManager->addCommand<OV_glBindTexture>(target, texture);
    glBindTexture(target, texture);
}