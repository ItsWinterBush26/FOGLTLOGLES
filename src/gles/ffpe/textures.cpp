#include "es/ffpe/lists.hpp"
#include "es/ffpe/states.hpp"
#include "gles/ffp/main.hpp"
#include "main.hpp"
#include "utils/log.hpp"

#include <GLES3/gl32.h>

void glClientActiveTexture(GLenum unit);

void FFP::registerTextureFunctions() {
    REGISTER(glClientActiveTexture);
}

void glClientActiveTexture(GLenum unit) {
    LOGI("glClientActiveTexture : unit=%u", unit);
    FFPE::List::addCommand<glClientActiveTexture>(unit);
    FFPE::States::ClientState::currentTexCoordUnit = unit;
    FFPE::States::ClientState::texCoordArrayTexUnits.push_back(unit);
}
