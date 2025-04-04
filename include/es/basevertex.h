#pragma once

#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "utils/fast_map.h"

#include <GLES3/gl32.h>
#include <omp.h>
#include <unordered_map>
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
    GLuint ebo;

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &ebo);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &ebo);
    }

    void batch(
        GLenum mode,
        const GLsizei* count,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        if (!drawcount) return;
        LOGI("orignal drawcount %d", drawcount);

        // Group draw calls by currently bound VBO
        FAST_MAP_BI(GLuint, std::vector<GLsizei>) drawGroups;
        FAST_MAP_BI(GLuint, std::vector<const void*>) indexGroups;
        FAST_MAP_BI(GLuint, std::vector<GLint>) baseVertexGroups;

        // Populate draw groups by checking the buffer states
        for (GLsizei i = 0; i < drawcount; ++i) {
            GLuint currentVBO = trackedStates->boundBuffers[GL_ARRAY_BUFFER];
            
            drawGroups[currentVBO].push_back(count[i]);
            indexGroups[currentVBO].push_back(indices[i]);
            baseVertexGroups[currentVBO].push_back(basevertex[i]);
        }

        GLuint lastBoundVBO = trackedStates->boundBuffers[GL_ARRAY_BUFFER];
        for (const auto& group : drawGroups) {
            GLuint vbo = group.first;

            // Bind only if necessary (we compare with the last bound buffer tracked in trackedStates)
            if (vbo != lastBoundVBO) {
                OV_glBindBuffer(GL_ARRAY_BUFFER, vbo);
                lastBoundVBO = vbo;
            }

            const std::vector<GLsizei>& groupCounts = group.second;
            const std::vector<const void*>& groupIndices = indexGroups[vbo];
            const std::vector<GLint>& groupBaseVertices = baseVertexGroups[vbo];

            LOGI("new 'drawcount' per se : %d", groupCounts.size());
            for (size_t i = 0; i < groupCounts.size(); ++i) {
                glDrawElementsBaseVertex(mode, groupCounts[i], type, groupIndices[i], groupBaseVertices[i]);
            }
        }
    }
};


inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
