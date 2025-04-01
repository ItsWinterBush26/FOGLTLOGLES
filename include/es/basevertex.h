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

        std::vector<indirect_pass_t> commands; // Accumulate all commands here.

indirect_pass_t current = {
    static_cast<GLuint>(counts[0]),
    1,
    static_cast<GLuint>(reinterpret_cast<uintptr_t>(indices[0]) / getTypeSize(type)),
    basevertex[0],
    0
};

for (GLsizei i = 1; i < drawcount; ++i) {
    GLuint nextFirstIndex = static_cast<GLuint>(reinterpret_cast<uintptr_t>(indices[i]) / getTypeSize(type));

    // Check if current command can be merged with the next one
    if (current.firstIndex + current.count == nextFirstIndex && current.baseVertex == basevertex[i]) {
        current.count += static_cast<GLuint>(counts[i]);
    } else {
        commands.push_back(current); // Push the current command
        current.count = static_cast<GLuint>(counts[i]);
        current.firstIndex = nextFirstIndex;
        current.baseVertex = basevertex[i];
    }
}
commands.push_back(current); // Push the last command

SaveBoundedBuffer sbb(GL_DRAW_INDIRECT_BUFFER);
OV_glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(indirect_pass_t) * commands.size(), commands.data(), GL_STREAM_DRAW);
        
        for (GLint i = 0; i < commands.size(); ++i) {
            glDrawElementsIndirect(mode, type, reinterpret_cast<const void*>(i * sizeof(indirect_pass_t)));
        }
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
