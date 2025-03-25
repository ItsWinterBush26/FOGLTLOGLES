#pragma once

#include "utils/pointers.h"

#include <GLES3/gl3.h>
#include <memory>

inline GLint getTypeSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:  return 1;
        case GL_UNSIGNED_SHORT: return 2;
        case GL_UNSIGNED_INT:   return 4;
        default:                return 0;
    }
}

struct MDElementsBuffer {
    GLuint buffer;

    bool usable;

    MDElementsBuffer() {
        glGenBuffers(1, &buffer);

        if (!glGetError()) usable = true;
    }

    ~MDElementsBuffer() {
        glDeleteBuffers(1, &buffer);
    }

    void batch(
        GLsizei totalCount, GLint typeSize,
        GLsizei primcount,
        const GLsizei* count, const void* const* indices
    ) {
        if (!usable) return;

        glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, totalCount * typeSize, nullptr, GL_STREAM_DRAW);

        GLsizei offset = 0;
        for (GLsizei i = 0; i < primcount; ++i) {
            if (!count[i]) continue;

            GLsizei dataSize = count[i] * typeSize;
            glBufferSubData(GL_COPY_WRITE_BUFFER, offset, dataSize, indices[i]);
            offset += dataSize;  
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }
};

inline std::shared_ptr<MDElementsBuffer> batcher = MakeAggregateShared<MDElementsBuffer>();
