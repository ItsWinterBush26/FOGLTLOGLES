#include "gles30/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);

void GLES30::registerBaseVertexFunction() {
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
    #pragma omp parallel for if(drawcount > 32)
    for(GLsizei i = 0; i < drawcount; i++) {
        #pragma omp critical
        {
            glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
        }
    }
}
