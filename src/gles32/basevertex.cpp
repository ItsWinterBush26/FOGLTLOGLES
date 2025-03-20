#include "gles32/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);

void GLES32::registerBaseVertexFunction() {
    REGISTER(glMultiDrawElementsBaseVertex);
}

void glMultiDrawElementsBaseVertex(
    GLenum mode,
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    if (drawcount == 0) return;

    for(GLsizei i = 0; i < drawcount; i++) {
        glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
    }
}
