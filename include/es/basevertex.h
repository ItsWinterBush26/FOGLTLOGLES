#pragma once

#include "es/binding_saver.h"

#include <GLES3/gl32.h>
// #include <omp.h>
#include <memory>
#include <vector>

inline GLuint findMaxIndex(const GLuint* indices, GLsizei count) {
    GLuint maxIndex;
    #pragma omp parallel for ordered
    for (GLsizei i = 0; i < count; ++i) {
        if (indices[i] > maxIndex) {
            #pragma omp ordered
            {
                maxIndex = indices[i]; // Update maxIndex if a larger index is found
            }
        }
    }
    return maxIndex;
}

struct MDElementsBaseVertexBatcher {
    GLuint vboBuffer, ebeBuffer;
    bool usable;

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &vboBuffer);
        glGenBuffers(1, &ebeBuffer);

        usable = (glGetError() == GL_NO_ERROR);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &vboBuffer);
        glDeleteBuffers(1, &ebeBuffer);
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
        int threadSize = std::min(omp_get_max_threads(), std::max(1, drawcount / 128));

        std::vector<size_t> vertexOffsets(drawcount);
        std::vector<size_t> indexOffsets(drawcount);
        std::vector<GLuint> maxIndices(drawcount);
        size_t totalVertices = 0, totalIndices = 0;
    
        // First pass: Sequential pre-calculation
        #pragma omp parallel for \
            schedule(static, drawcount / threadSize) \
            reduction(+:totalVertices, totalIndices) \
            num_threads(threadSize)
        for (int i = 0; i < drawcount; ++i) {
            const GLuint* index = static_cast<const GLuint*>(indices[i]);
            GLuint maxIndex = findMaxIndex(index, counts[i]);
            
            vertexOffsets[i] = totalVertices;
            totalVertices += (maxIndex + 1); // Vertices needed for this draw
            
            indexOffsets[i] = totalIndices;
            totalIndices += counts[i];

            maxIndices[i] = maxIndex;
        }
    
        // Preallocate memory for merged data
        std::vector<float> mergedVertices(totalVertices * vertexStride);
        std::vector<GLuint> mergedIndices(totalIndices);
    
        // Second pass: Parallel data processing
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < drawcount; ++i) {
            // Get original data from shadow copies (no OpenGL calls)
            const GLuint* origIndices = get_indices_from_shadow_copy(...);
            const float* origVertices = get_vertices_from_shadow_copy(...);
    
            // Copy vertices to preallocated position
            size_t vertDstOffset = vertexOffsets[i] * vertexStride;
            
            std::memcpy(
                &mergedVertices[vertDstOffset],
                origVertices,
                (maxIndices[i] + 1) * vertexStride * sizeof(float)
            );
    
            // Adjust indices and write to preallocated position
            size_t idxDstOffset = indexOffsets[i];
            for (int j = 0; j < counst[i]; ++j) {
                mergedIndices[idxDstOffset + j] = origIndices[j] + vertexOffsets[i]; // Index adjustment
            }
        }    
    

        SaveBoundedBuffer sbb(GL_ARRAY_BUFFER);
        OV_glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
        glBufferData(GL_ARRAY_BUFFER, mergedVertices.size() * sizeof(float), mergedVertices.data(), GL_STATIC_DRAW);
    
        SaveBoundedBuffer sbb2(GL_ELEMENT_ARRAY_BUFFER);
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebeBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mergedIndices.size() * sizeof(GLuint), mergedIndices.data(), GL_STATIC_DRAW);
    
        // Issue a single draw call with the merged data
        glDrawElementsBaseVertex(mode, mergedIndices.size(), GL_UNSIGNED_INT, 0, 0);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
