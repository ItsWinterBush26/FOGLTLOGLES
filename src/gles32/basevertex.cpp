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
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    if (drawcount <= 0) return;

    GLsizei totalIndices = 0;
    for (GLsizei i = 0; i < drawcount; i++) {
        totalIndices += count[i];
    }

    switch(type) {
        case GL_UNSIGNED_BYTE: {
            std::vector<GLubyte> mergedIndices;
            mergeIndices(count, indices, drawcount, basevertex, mergedIndices, totalIndices);
            batcher->batch(mode, type, mergedIndices, totalIndices);
            break;
        }

        case GL_UNSIGNED_SHORT: {
            std::vector<GLushort> mergedIndices;
            mergeIndices(count, indices, drawcount, basevertex, mergedIndices, totalIndices);
            batcher->batch(mode, type, mergedIndices, totalIndices);
            break;
        }

        case GL_UNSIGNED_INT: {
            std::vector<GLuint> mergedIndices;
            mergeIndices(count, indices, drawcount, basevertex, mergedIndices, totalIndices);
            batcher->batch(mode, type, mergedIndices, totalIndices);
            break;
        }

        default:
            // Unsupported type
            break;
    }
}