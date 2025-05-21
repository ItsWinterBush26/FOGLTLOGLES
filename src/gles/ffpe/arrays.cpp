#include "es/ffp.h"
#include "es/ffpe/draw.h"
#include "es/utils.h"
#include "gles/ffp/enums.h"
#include "gles/ffp/main.h"
#include "gles/ffp/arrays.h"
#include "main.h"

#include <GLES3/gl32.h>

void FFP::registerArrayFunctions() {
    REGISTER(glEnableClientState);
    REGISTER(glDisableClientState);

    REGISTER(glVertexPointer);
    REGISTER(glColorPointer);

    FFPE::Rendering::Arrays::init();
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    Lists::displayListManager->addCommand<glVertexPointer>(size, type, stride, pointer);
    FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY)->parameters = {
        trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer != 0,
        size, type,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    Lists::displayListManager->addCommand<glColorPointer>(size, type, stride, pointer);
    FFPE::States::ClientState::Arrays::getArray(GL_COLOR_ARRAY)->parameters = {
        trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer != 0,
        size, type,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    Lists::displayListManager->addCommand<glTexCoordPointer>(size, type, stride, pointer);
    FFPE::States::ClientState::Arrays::getArray(GL_TEXTURE_COORD_ARRAY)->parameters = {
        trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer != 0,
        size, type,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glEnableClientState(GLenum array) {
    Lists::displayListManager->addCommand<glEnableClientState>(array);
    FFPE::States::ClientState::Arrays::getArray(array)->enabled = true;
}

void glDisableClientState(GLenum array) {
    Lists::displayListManager->addCommand<glDisableClientState>(array);
    FFPE::States::ClientState::Arrays::getArray(array)->enabled = false;
}

