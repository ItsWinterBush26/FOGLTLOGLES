#pragma once

#include "es/binding_saver.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl3.h>
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

struct MDElementsBatcher {
    GLuint buffer;

    bool usable;

    MDElementsBatcher() {
        glGenBuffers(1, &buffer);

        usable = (glGetError() == GL_NO_ERROR);
    }

    ~MDElementsBatcher() {
        glDeleteBuffers(1, &buffer);
    }

    void batch(
        GLsizei totalCount, GLint typeSize,
        GLsizei primcount,
        const GLsizei* count, const void* const* indices,
        SaveBoundedBuffer sbb
    ) {
        if (!usable) return;
        int threadSize = std::min(omp_get_max_threads(), std::max(1, primcount / 64));

        OV_glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, totalCount * typeSize, nullptr, GL_STREAM_DRAW);

        GLsizei offset = 0;
        #pragma omp parallel for \
            if(primcount > 256) \
            schedule(static, 1) \
            reduction(+:offset) \
            ordered
        for (GLsizei i = 0; i < primcount; ++i) {
            if (!count[i]) continue;
            #pragma omp ordered
            {
                GLsizei dataSize = count[i] * typeSize;
            
                if (sbb.boundedBuffer != 0) {
                    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)indices[i], offset, dataSize)    
                } else {
                    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, dataSize, indices[i]);
                }
            
                offset += dataSize;
            }
        }

        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }
};

inline std::shared_ptr<MDElementsBatcher> batcher;
