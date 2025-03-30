#pragma once

#include "es/binding_saver.h"
#include "gles20/buffer_tracking.h"
#include <GLES3/gl32.h>
#include <memory>

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

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &buffer);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &buffer);
    }

    template<typename T>
    void batch(GLenum mode, GLenum type, std::vector<T>& mergedIndices, GLsizei totalIndices) {
        SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);

        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            mergedIndices.size() * sizeof(T),
            mergedIndices.data(),
            GL_STATIC_DRAW);

        glDrawElements(mode, totalIndices, type, nullptr);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;

template<typename T>
inline void mergeIndices(const GLsizei* count, const void* const* indices, GLsizei drawcount, const GLint* basevertex, std::vector<T>& mergedIndices, GLsizei totalIndices) {
    mergedIndices.reserve(totalIndices);
    for (GLsizei i = 0; i < drawcount; i++) {
        if (!indices[i]) continue;
        
        // indices[i] is a pointer to T elements.
        const T* idx = static_cast<const T*>(indices[i]);
        for (GLsizei j = 0; j < count[i]; j++) {
            // Adjust each index by the corresponding basevertex.
            mergedIndices.push_back(idx[j] + static_cast<T>(basevertex[i]));
        }
    }
}
