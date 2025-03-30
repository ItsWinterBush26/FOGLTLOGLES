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
    if (drawcount <= 0) return;

    GLint typeSize = getTypeSize(type);
    if (typeSize == 0) return; // Unsupported type

    GLsizei totalCount = 0;
    for (GLsizei i = 0; i < drawcount; ++i) totalCount += counts[i];
    if (totalCount == 0) return;

    SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
    batcher->prepareBatch(counts, totalCount, typeSize, drawcount, indices, sbb);
    
    glDrawElementsBaseVertex(mode, totalCount, type, (void*)0, basevertex[0]); // Use basevertex[0] as the base
}
