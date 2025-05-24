#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>

void glClientActiveTexture(GLenum unit);

void FFP::registerTextureFunctions() {
    REGISTER(glClientActiveTexture);
}

void glClientActiveTexture(GLenum unit) {
    LOGI("glClientActiveTexture : unit=%u", unit);
    Lists::displayListManager->addCommand<glClientActiveTexture>(unit);
    FFPE::States::ClientState::currentTexCoordUnit = unit;
    FFPE::States::ClientState::texCoordArrayTexUnits.push_back(unit);
}
