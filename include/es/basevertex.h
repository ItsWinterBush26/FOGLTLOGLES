#pragma once

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl32.h>
#include <memory>
#include <omp.h>
#include <vector>

inline GLint getTypeSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:  return 1;
        case GL_UNSIGNED_SHORT: return 2;
        case GL_UNSIGNED_INT:   return 4;
        default:                return 0;
    }
}

struct DrawElementsBaseVertexCommand {
    GLsizei count;
    uintptr_t firstIndex;
    GLint baseVertex;
};

struct MDElementsBaseVertexBatcher {
    // GLuint indirectBuffer;
    
    MDElementsBaseVertexBatcher() {
        // glGenBuffers(1, &indirectBuffer);

        /* SaveBoundedBuffer sbb(GL_DRAW_INDIRECT_BUFFER);
        OV_glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, 512 * sizeof(indirect_pass_t), nullptr, GL_STREAM_DRAW); */
    }

    ~MDElementsBaseVertexBatcher() {
        // glDeleteBuffers(1, &indirectBuffer);
    }

    void batch(
        GLenum mode,
        const GLsizei* counts,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        if (drawcount <= 0) return;
        
        GLint typeSize = getTypeSize(type);
        if (typeSize == 0) return;

        DrawElementsBaseVertexCommand current = {
            counts[0],
            reinterpret_cast<uintptr_t>(indices[0]) / typeSize,
            basevertex[0]
        };
        
        for (GLsizei i = 1; i < drawcount; ++i) {
            GLuint nextFirstIndex = reinterpret_cast<uintptr_t>(indices[i]) / typeSize;

             if (current.firstIndex + current.count == nextFirstIndex && current.baseVertex == basevertex[i]) {
                current.count += counts[i];
                LOGI("Merged!");
            } else {
                glDrawElementsBaseVertex(mode, current.count, type, reinterpret_cast<const void*>(current.firstIndex), current.baseVertex);

                current.count = counts[i];
                current.firstIndex = nextFirstIndex * typeSize;
                current.baseVertex = basevertex[i];
            }
        }

        glDrawElementsBaseVertex(mode, current.count, type, reinterpret_cast<const void*>(current.firstIndex), current.baseVertex);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
