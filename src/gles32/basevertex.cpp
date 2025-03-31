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
    batcher->batch(
        mode,
        count,
        type,
        indices,
        drawcount,
        basevertex
    );
}
