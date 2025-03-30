#pragma once

#include "es/binding_saver.h"
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

struct MDElementsBaseVertexBatcher {
    GLuint buffer;

    bool usable;

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &buffer);

        usable = (glGetError() == GL_NO_ERROR);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &buffer);
    }

    void prepareBatch(
        const GLsizei* counts,
        GLsizei totalCount,
        GLsizei typeSize,
        GLsizei drawcount,
        const void* const* indices,
        SaveBoundedBuffer& sbb
    ) {
        OV_glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, totalCount * typeSize, nullptr, GL_STREAM_DRAW);

        std::vector<GLsizei> offsets(drawcount);
        GLsizei runningOffset = 0;
        for (GLsizei i = 0; i < drawcount; ++i) {
            offsets[i] = runningOffset;
            runningOffset += counts[i] * typeSize;
        }

        #pragma omp parallel for if (drawcount > 128) schedule(static, 1) num_threads(std::min(omp_get_max_threads(), std::max(1, drawcount / 64)))
        for (GLsizei i = 0; i < drawcount; ++i) {
            if (!counts[i]) continue;

            GLsizei dataSize = counts[i] * typeSize;
            if (sbb.boundedBuffer != 0) {
                glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, reinterpret_cast<GLintptr>(indices[i]), offsets[i], dataSize);
            } else {
                glBufferSubData(GL_COPY_WRITE_BUFFER, offsets[i], dataSize, indices[i]);
            }
        }

        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
