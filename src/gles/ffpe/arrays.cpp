#include "es/ffpe/states.hpp"
#include "es/ffpe/draw.hpp"
#include "es/utils.hpp"
#include "gles/ffp/enums.hpp"
#include "gles/ffp/main.hpp"
#include "gles/ffp/arrays.hpp"
#include "main.hpp"

#include <GLES3/gl32.h>

void FFP::registerArrayFunctions() {
    REGISTER(glEnableClientState);
    REGISTER(glDisableClientState);

    REGISTER(glVertexPointer);
    REGISTER(glColorPointer);
    REGISTER(glTexCoordPointer);

    FFPE::Rendering::Arrays::init();
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    LOGI("glVertexPointer : size=%i mode=%u stride=%i start=%p", size, type, stride, pointer);
    FFPE::States::Manager::markStateAsDirty();
    
    FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY)->parameters = {
        !stride, size, type, GL_FALSE,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    LOGI("glColorPointer : size=%i mode=%u stride=%i start=%p", size, type, stride, pointer);
    FFPE::States::Manager::markStateAsDirty();
    
    FFPE::States::ClientState::Arrays::getArray(GL_COLOR_ARRAY)->parameters = {
        !stride, size, type, GL_TRUE,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    LOGI("glTexCoordPointer : size=%i mode=%u stride=%i start=%p", size, type, stride, pointer);
    FFPE::States::Manager::markStateAsDirty();
    
    FFPE::States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0)->parameters = {
        !stride, size, type, GL_FALSE,
        stride ? stride : size * ESUtils::TypeTraits::getTypeSize(type),
        pointer
    };
}

void glEnableClientState(GLenum array) {
    switch (array) {
        case GL_TEXTURE_COORD_ARRAY:
            FFPE::States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0)->enabled = true;
        return;

        default:
            FFPE::States::ClientState::Arrays::getArray(array)->enabled = true;
        return;
    }

    FFPE::States::Manager::markStateAsDirty();
}

void glDisableClientState(GLenum array) {
    switch (array) {
        case GL_TEXTURE_COORD_ARRAY:
            FFPE::States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0)->enabled = false;
        return;

        default:
            FFPE::States::ClientState::Arrays::getArray(array)->enabled = false;
        return;
    }

    FFPE::States::Manager::markStateAsDirty();
}

