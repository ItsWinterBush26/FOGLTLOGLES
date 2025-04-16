#include "es/basevertex.h"
#include "gles32/main.h"
#include "main.h"
#include "utils/pointers.h"

#include <GLES3/gl32.h>

void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);

void GLES32::registerBaseVertexFunction() {
    REGISTER(glMultiDrawElementsBaseVertex);

    batcher = MakeAggregateShared<MDElementsBaseVertexBatcher>();
}

void glMultiDrawElementsBaseVertex(
    GLenum mode,
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    if (drawcount <= 0) return;

    for (GLint i = 0; i < drawcount; ++i) {
        if (count[i] > 0) glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
    }
    
    /*
    batcher->batch(
        mode,
        count,
        type,
        indices,
        drawcount,
        basevertex
    ); */
}
