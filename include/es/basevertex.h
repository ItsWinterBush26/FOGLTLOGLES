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

        GLuint totalCount;
        GLuint totalFirstIndex;
        GLint totalBaseVertex;
        
        #pragma omp parallel for schedule(static, 2) num_threads(4) if(drawcount > 256) \
            reduction(+:totalCount) \
            reduction(+:totalFirstIndex) \
            reduction(+:totalBaseVertex)
        for (GLsizei i = 0; i < drawcount; ++i) {
            totalCount += static_cast<GLuint>(counts[i]);
            totalFirstIndex += static_cast<GLuint>(reinterpret_cast<uintptr_t>(indices[i]) / typeSize);
            totalBaseVertex += basevertex[i];
        }

        indirect_pass_t command = {
            totalCount,
            1,
            totalFirstIndex,
            totalBaseVertex,
            0
        };
        
        SaveBoundedBuffer sbb(GL_DRAW_INDIRECT_BUFFER);
        OV_glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(indirect_pass_t), &command, GL_STREAM_DRAW);

        glDrawElementsIndirect(mode, type, reinterpret_cast<const void*>(0));
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
