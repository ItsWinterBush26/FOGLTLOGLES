#include "es/basevertex.h"
#include "gles32/main.h"
#include "main.h"
#include "utils/pointers.h"

#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <vector>

void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);

void GLES32::registerBaseVertexFunction() {
    REGISTER(glMultiDrawElementsBaseVertex);

    batcher = MakeAggregateShared<MDElementsBaseVertexBatcher>();
}

void glMultiDrawElementsBaseVertex(
    GLenum mode,
    const GLsizei* counts,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    batcher->batchAndDraw(mode, counts, type, indices, drawcount, basevertex);
}
