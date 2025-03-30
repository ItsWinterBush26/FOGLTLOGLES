#include "es/multidraw.h"
#include "es/binding_saver.h"
#include "gles20/buffer_tracking.h"
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
    
    // Safe approach: track consecutive draws with the same basevertex
    for (GLsizei i = 0; i < drawcount;) {
        GLint currentBaseVertex = basevertex[i];
        
        // Count how many consecutive draws have the same basevertex
        GLsizei sameBaseVertexCount = 0;
        GLsizei totalVertices = 0;
        
        for (GLsizei j = i; j < drawcount && basevertex[j] == currentBaseVertex; j++) {
            sameBaseVertexCount++;
            totalVertices += count[j];
        }
        
        // If only one draw or very few vertices, just do direct draw
        if (sameBaseVertexCount == 1 || totalVertices < 100) {
            for (GLsizei j = i; j < i + sameBaseVertexCount; j++) {
                if (count[j] > 0) {
                    glDrawElementsBaseVertex(mode, count[j], type, indices[j], basevertex[j]);
                }
            }
        } else {
            // Try to batch, but be very careful about buffer state
            SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
            GLuint tempBuffer;
            glGenBuffers(1, &tempBuffer);
            
            // Use a temporary buffer for safety
            OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
            
            // Simple batching without using complex batcher mechanism
            GLsizei typeSize = getTypeSize(type);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalVertices * typeSize, NULL, GL_STREAM_DRAW);
            
            GLsizei offset = 0;
            for (GLsizei j = i; j < i + sameBaseVertexCount; j++) {
                if (count[j] > 0) {
                    GLsizei dataSize = count[j] * typeSize;
                    // Only use glBufferSubData approach for simplicity and safety
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, dataSize, indices[j]);
                    offset += dataSize;
                }
            }
            
            // Draw all vertices in one call
            glDrawElementsBaseVertex(mode, totalVertices, type, (void*)0, currentBaseVertex);
            
            // Clean up
            glDeleteBuffers(1, &tempBuffer);
            sbb.restore();
        }
        
        // Move to next group
        i += sameBaseVertexCount;
    }
}
