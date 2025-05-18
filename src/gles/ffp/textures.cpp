#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glClientActiveTexture(GLenum unit);

void FFP::registerTextureFunctions() {
    REGISTER(glClientActiveTexture);
}

void glClientActiveTexture(GLenum unit) {
    FFPE::States::ClientState::currentTexCoordTextureUnit = unit;
}
