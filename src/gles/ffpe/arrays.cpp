#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glEnableClientState(GLenum array);
void glDisableClientState(GLenum array);

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer);

void FFP::registerArrayFunctions() {
    REGISTER(glEnableClientState);
    REGISTER(glDisableClientState);

    REGISTER(glVertexPointer);
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer) {
    FFPE::States::ClientState::Arrays::arrayStates[GL_VERTEX_ARRAY].parameters = {
        size, type, stride, (GLuint) (uintptr_t) pointer
    };
}

void glEnableClientState(GLenum array) {
    FFPE::States::ClientState::Arrays::arrayStates[array].enabled = true;
}

void glDisableClientState(GLenum array) {
    FFPE::States::ClientState::Arrays::arrayStates[array].enabled = false;
}

