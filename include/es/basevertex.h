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

struct indirect_pass_t {
    GLuint count;
    GLuint instanceCount = 1;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint reservedMustBeZero;
};

struct MDElementsBaseVertexBatcher {
    GLuint indirectBuffer;
    
    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &indirectBuffer);

        /* SaveBoundedBuffer sbb(GL_DRAW_INDIRECT_BUFFER);
        OV_glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, 512 * sizeof(indirect_pass_t), nullptr, GL_STREAM_DRAW); */
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &indirectBuffer);
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
        /* if (drawcount > 128) {

        } */
        
        GLint typeSize = getTypeSize(type);
        if (typeSize == 0) return;
        if (trackedStates->boundBuffers[GL_ELEMENT_ARRAY_BUFFER] == 0) return;

        indirect_pass_t current = {
            static_cast<GLuint>(counts[0]),
            1,
            static_cast<GLuint>(reinterpret_cast<uintptr_t>(indices[0]) / getTypeSize(type)),
            basevertex[0],
            0
        };

        SaveBoundedBuffer sbb(GL_DRAW_INDIRECT_BUFFER);
        OV_glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        
        for (GLsizei i = 1; i < drawcount; ++i) {
            GLuint nextFirstIndex = static_cast<GLuint>(reinterpret_cast<uintptr_t>(indices[i]) / getTypeSize(type));

             if (current.firstIndex + current.count == nextFirstIndex && current.baseVertex == basevertex[i]) {
                current.count += static_cast<GLuint>(counts[i]);
            } else {
                glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(indirect_pass_t), &current, GL_STREAM_DRAW);
                glDrawElementsIndirect(mode, type, reinterpret_cast<const void*>(0));

                current.count = static_cast<GLuint>(counts[i]);
                current.firstIndex = nextFirstIndex;
                current.baseVertex = basevertex[i];
            }
        }
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(indirect_pass_t), &current, GL_STREAM_DRAW);
        glDrawElementsIndirect(mode, type, reinterpret_cast<const void*>(0));
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
