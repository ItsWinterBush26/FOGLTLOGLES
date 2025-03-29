#include "es/multidraw.h"
#include "es/binding_saver.h"
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
    if (drawcount <= 0) return;

    GLint typeSize = getTypeSize(type);
    if (typeSize == 0) return; // Unsupported type
    
    SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
    
    // Group by basevertex value
    GLint lastBaseVertex = basevertex[0];
    GLsizei startIdx = 0;
    
    for (GLsizei i = 1; i <= drawcount; ++i) {
        // Process batch when basevertex changes or at the end
        if (i == drawcount || basevertex[i] != lastBaseVertex) {
            // Calculate total count for this batch
            GLsizei batchTotalCount = 0;
            for (GLsizei j = startIdx; j < i; ++j) {
                batchTotalCount += count[j];
            }
            
            if (batchTotalCount > 0) {
                // Combine all indices
                batcher->batch(
                    batchTotalCount, typeSize,
                    i - startIdx,
                    count + startIdx, indices + startIdx,
                    sbb
                );
                
                glDrawElementsBaseVertex(mode, batchTotalCount, type, (void*)0, lastBaseVertex);
            }
            
            // Start new batch
            if (i < drawcount) {
                lastBaseVertex = basevertex[i];
                startIdx = i;
            }
        }
    }
}
