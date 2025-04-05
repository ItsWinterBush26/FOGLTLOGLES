#pragma once

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <omp.h>
#include <vector>

// reinterpret_cast<size_t>(indices[i]) / sizeof(T)
template<typename T>
inline size_t getIndexOffsetFast(T indexOffset, size_t size) {
    switch (size) {
        case 4: return reinterpret_cast<size_t>(indexOffset) >> 2;
        case 2: return reinterpret_cast<size_t>(indexOffset) >> 1;
        default: return reinterpret_cast<size_t>(indexOffset);
    }
}

template<typename T>
inline std::vector<T> mergeIndicesCPU(
    const GLsizei* count,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    GLsizei totalCount = 0;
    #pragma omp parallel for reduction(+:totalCount)
    for (GLsizei i = 0; i < drawcount; ++i) {
        totalCount += count[i];
    }
    
    std::vector<T> mergedIndices;

    LOGI("reserve %d for mergedIndices", totalCount);
    mergedIndices.reserve(totalCount);

    for (GLsizei i = 0; i < drawcount; ++i) {
        const T* indexData = static_cast<const T*>(indices[i]);
        const GLint indexBaseVertex = basevertex[i];

        for (GLsizei j = 0; j < count[i]; ++j) {
            mergedIndices.push_back(indexData[j] + indexBaseVertex);
        }
    }

    return mergedIndices;
}

template<typename T>
inline std::vector<T> mergeIndicesGPU(
    const GLsizei* count,
    const void* const* indices, // an offset now, not an array of pointers
    GLsizei drawcount,
    const GLint* basevertex
) {
    GLsizei totalCount = 0;
    #pragma omp parallel for reduction(+:totalCount)
    for (GLsizei i = 0; i < drawcount; ++i) {
        totalCount += count[i];
    }

    std::vector<T> mergedIndices;
    mergedIndices.reserve(totalCount);

    for (GLsizei i = 0; i < drawcount; ++i) {
        LOGI("Mapping indexData. offset=%li, size=%u", (GLintptr) indices[i], count[i]);
        void* realIndices = glMapBufferRange(
            GL_ELEMENT_ARRAY_BUFFER,
            (GLintptr) indices[i],
            count[i] * sizeof(T),
            GL_MAP_READ_BIT
        );

        if (!realIndices) {
            LOGE("glMapBufferRange failed!"); 
            LOGE("%u", glGetError());
            return mergedIndices;
        }
        
        const T* indexData = static_cast<const T*>(realIndices);
        const GLint indexBaseVertex = basevertex[i];
        
        for (GLsizei j = 0; j < count[i]; ++j) {
            mergedIndices.push_back(indexData[j] + indexBaseVertex);
        }

        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }
    mergedIndices.shrink_to_fit();

    LOGI("W MERGE SYCCES (indices as offsets)");
    return mergedIndices;
}

template<typename T>
inline void drawActual(
    GLenum mode,
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex,
    GLuint buffer
) {
    std::vector<T> mergedIndices;
    if (trackedStates->boundBuffers[GL_ELEMENT_ARRAY_BUFFER].buffer == 0) {
        mergedIndices = mergeIndicesCPU<T>(count, indices, drawcount, basevertex);
    } else {
        mergedIndices = mergeIndicesGPU<T>(count, indices, drawcount, basevertex);
    }
    
    if (mergedIndices.empty()) {
        LOGE("mergedIndices is empty");
        return;
    }

    SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mergedIndices.size() * sizeof(T),
        mergedIndices.data(),
        GL_STATIC_DRAW
    );
    
    glDrawElementsBaseVertex(
        mode,
        mergedIndices.size(),
        type,
        0, // no offset, we already have merged the indices
        0
    );
    LOGI("called draw!");
    /* LOGI("indices:");
    for (const auto& index : mergedIndices) {
        LOGI("%u", index);
    } */
}  

struct MDElementsBaseVertexBatcher {
    GLuint eab;

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &eab);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &eab);
    }

    void batch(
        GLenum mode,
        const GLsizei* count,
        GLenum type,
        const void* const* indices, // array of pointers to indices array (apparently sometimes)
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        if (!drawcount) return;
        
        // plan:                           the second arr in the indices
        // merge indices                            |  |  |
        // [0, 1, 2] | [0, 1, 2] turns to [0, 1, 2, 3, 4, 5]
        //              |  |  |
        //           the second arr
        //
        // indices (const void* const*):
        // [ptr1, ptr2]
        // ptr1: [0, 1, 2]
        // ptr2: [0, 1, 2]
        //
        // glDrawElementsBaseVertex(.., .., .., indices[i], ..);
        //                                              |
        //                              array pointer or an offset (wth ogl)
        //                                         const void*

        switch (type) {
            case GL_UNSIGNED_BYTE: {
                drawActual<GLubyte>(
                    mode,
                    count,
                    type,
                    indices,
                    drawcount,
                    basevertex,
                    eab
                );
                break;
            }
            case GL_UNSIGNED_SHORT: {
                drawActual<GLushort>(
                    mode,
                    count,
                    type,
                    indices,
                    drawcount,
                    basevertex,
                    eab
                );
                break;
            }
            case GL_UNSIGNED_INT: {
                drawActual<GLuint>(
                    mode,
                    count,
                    type,
                    indices,
                    drawcount,
                    basevertex,
                    eab
                );
                break;
            }
            default:
                return;
        }
    }
};


inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
