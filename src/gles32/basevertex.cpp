#include "gles32/main.h"
#include "main.h"
#include "utils/pointers.h"

#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <omp.h>
#include <vector>

void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);

void GLES32::registerBaseVertexFunction() {
    REGISTER(glMultiDrawElementsBaseVertex);

    // batcher = MakeAggregateShared<MDElementsBaseVertexBatcher>();
}

void glMultiDrawElementsBaseVertex(
    GLenum mode,
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount,
    const GLint* basevertex
) {
    if (drawcount <= 0) return;
    GLsizei threadNum = std::min(omp_get_max_threads(), std::max(1, drawcount / 64));

    // Calculate the total number of indices across all draw calls.
    GLsizei totalIndexCount = 0;

    #pragma omp parallel for \
        if(drawcount > 512) \
        schedule(static, drawcount / threadNum) \
        num_threads(threadNum) \
        reduction(+:totalIndexCount)
    for (GLsizei i = 0; i < drawcount; ++i) {
        totalIndexCount += count[i];
    }

    // We'll merge the indices into a temporary vector.
    // We use uint32_t for the merged indices since we've adjusted the values.
    std::vector<uint32_t> mergedIndices;
    mergedIndices.reserve(static_cast<size_t>(totalIndexCount));

    // For each draw, adjust indices by its corresponding basevertex and append.
    #pragma omp parallel for \
        if(drawcount > 256) \
        schedule(static, 1)
    for (GLsizei i = 0; i < drawcount; ++i) {
        GLint base = basevertex[i];

        #pragma omp parallel for \
            if(drawcount > 256) \
            schedule(static, 1) \
            shared(base)
        for (GLsizei j = 0; j < count[i]; ++j) {
            uint32_t index = 0;
            switch (type) {
                case GL_UNSIGNED_INT: {
                    const uint32_t* u32 = reinterpret_cast<const uint32_t*>(indices[i]);
                    index = u32[j] + base;
                    break;
                }
                case GL_UNSIGNED_SHORT: {
                    const uint16_t* u16 = reinterpret_cast<const uint16_t*>(indices[i]);
                    index = static_cast<uint32_t>(u16[j]) + base;
                    break;
                }
                case GL_UNSIGNED_BYTE: {
                    const uint8_t* u8 = reinterpret_cast<const uint8_t*>(indices[i]);
                    index = static_cast<uint32_t>(u8[j]) + base;
                    break;
                }
                default:
                    // Unsupported index type.
                    break;
            }
            mergedIndices.push_back(index);
        }
    }

    // Now that the indices have been merged and adjusted, issue a single draw call.
    // Note: Since we've already applied base offsets, we pass 0 as the base vertex.
    glDrawElementsBaseVertex(
        mode,
        static_cast<GLsizei>(mergedIndices.size()),
        GL_UNSIGNED_INT,
        mergedIndices.data(),
        0
    );
}
