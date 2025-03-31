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
        int threadSize = std::min(omp_get_max_thread(), std::max(1, primcount / 64));

        OV_glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, totalCount * typeSize, nullptr, GL_STREAM_DRAW);

        std::vector<GLsizei> offsets(primcount);
        if (primcount < 5000) {
            GLsizei runningOffset = 0;
            for (GLsizei i = 0; i < primcount; ++i) {
                offsets[i] = runningOffset;
                runningOffset += count[i] * typeSize;
            }
        } else {
            std::vector<GLsizei> localSums(threadSize, 0);

            #pragma omp parallel num_threads(threadSize)
            {
                int threadID = omp_get_thread_num();
                int start = (primcount * threadID) / threadSize;
                int end = (primcount * (threadID + 1)) / threadSize;

                GLsizei localSum = 0;

                #pragma omp for schedule(static)
                for (int i = 0; i < primcount; ++i) {
                    offsets[i] = localSum;
                    localSum += count[i] * typeSize;
                }

                localSums[threadID] = localSum;

                #pragma omp barrier  // Ensure all threads finish their local sums

                GLsizei globalOffset = 0;
                for (int j = 0; j < threadID; ++j) {
                    globalOffset += localSums[j];
                }

                #pragma omp for schedule(static)
                for (int i = 0; i < primcount; ++i) {
                    offsets[i] += globalOffset;
                }
            }
        }


        #pragma omp parallel for \
            if(primcount > 128) \
            schedule(static, primcount / threadSize) \
            num_threads(threadSize) // scale threads
        for (GLsizei i = 0; i < primcount; ++i) {
            if (!count[i]) continue;

            GLsizei dataSize = count[i] * typeSize;
            if (sbb.boundedBuffer != 0) {
                glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)indices[i], offsets[i], dataSize);
            } else {
                glBufferSubData(GL_COPY_WRITE_BUFFER, offsets[i], dataSize, indices[i]);
            }
        }

        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }
};

inline std::shared_ptr<MDElementsBatcher> batcher;
