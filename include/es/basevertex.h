#pragma once

#include "es/binding_saver.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl32.h>
#include <vector>
#include <cstring>
#include <memory>
#include <algorithm>

inline GLint getTypeSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:  return 1;
        case GL_UNSIGNED_SHORT: return 2;
        case GL_UNSIGNED_INT:   return 4;
        default:                return 0;
    }
}

inline GLuint getMaxIndex(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:  return 0xFF;
        case GL_UNSIGNED_SHORT: return 0xFFFF;
        case GL_UNSIGNED_INT:   return 0xFFFFFFFF;
        default:                return 0;
    }
}

template <typename T>
inline bool copyIndices(std::vector<T>& dst, GLuint& dstOffset, 
                   const T* src, GLsizei count) {
        if (dstOffset + count > dst.size()) {
            return false;
        }
        
        std::memcpy(&dst[dstOffset], src, count * sizeof(T));
        dstOffset += count;
        return true;
    }

template <typename T>
inline bool addRestartIndex(std::vector<T>& dst, GLuint& dstOffset, T restartVal) {
        if (dstOffset >= dst.size()) {
            return false;
        }
        
        dst[dstOffset] = restartVal;
        dstOffset++;
        return true;
    }

struct MDElementsBaseVertexBatcher {
    GLuint indexBuffer;
    bool usable;

    MDElementsBaseVertexBatcher() {
        glGenBuffers(1, &indexBuffer);
        usable = (glGetError() == GL_NO_ERROR);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteBuffers(1, &indexBuffer);
    }

    void batchAndDraw(
        GLenum mode,
        const GLsizei* counts,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        // Basic validation
        if (!usable) return;
        // if (!counts || !indices || !basevertex || drawcount <= 0) return;
        
        // const GLint typeSize = getTypeSize(type);
        // if (typeSize == 0) return;
        
        // For small batch counts, just use multiple direct draw calls
        // if (drawcount <= 4) {
        #pragma omp parallel for \
            schedule(static, 1) \
            num_threads(std::min(8, std::max(1, drawcount / 64))) // scale threads
        for (GLsizei i = 0; i < drawcount; ++i) {
            if (counts[i] < 1) continue;
            glDrawElementsBaseVertex(mode, counts[i], type, indices[i], basevertex[i]);
        }
            // return;
        // }
        /*
        // Calculate total indices including the primitive restart markers
        GLsizei totalIndices = 0;
        for (GLsizei i = 0; i < drawcount; ++i) {
            totalIndices += counts[i];
        }
        
        // Add space for primitive restart indices
        const GLsizei newTotalIndices = totalIndices + (drawcount - 1);
        
        // Process based on index type
        switch (type) {
            case GL_UNSIGNED_BYTE: {
                std::vector<GLubyte> combinedIndices(newTotalIndices);
                GLuint offset = 0;
                GLubyte restartIndex = 0xFF;
                
                for (GLsizei i = 0; i < drawcount; ++i) {
                    if (counts[i] <= 0) continue;
                    
                    // Get the source indices
                    const GLubyte* srcIndices = static_cast<const GLubyte*>(indices[i]);
                    
                    // Batch of indices for this draw call
                    for (GLsizei j = 0; j < counts[i]; ++j) {
                        // Add the basevertex offset
                        GLint adjustedIndex = srcIndices[j] + basevertex[i];
                        
                        // Clamp to valid range
                        adjustedIndex = std::max(0, std::min(adjustedIndex, 255));
                        
                        combinedIndices[offset++] = static_cast<GLubyte>(adjustedIndex);
                    }
                    
                    // Add primitive restart index between draws
                    if (i < drawcount - 1) {
                        combinedIndices[offset++] = restartIndex;
                    }
                }
                
                // Upload combined indices
                SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
                OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLubyte), 
                           combinedIndices.data(), GL_STREAM_DRAW);
                
                // Draw with primitive restart
                glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                glDrawElements(mode, offset, type, nullptr);
                glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                break;
            }
            
            case GL_UNSIGNED_SHORT: {
                std::vector<GLushort> combinedIndices(newTotalIndices);
                GLuint offset = 0;
                GLushort restartIndex = 0xFFFF;
                
                for (GLsizei i = 0; i < drawcount; ++i) {
                    if (counts[i] <= 0) continue;
                    
                    // Get the source indices
                    const GLushort* srcIndices = static_cast<const GLushort*>(indices[i]);
                    
                    // Batch of indices for this draw call
                    for (GLsizei j = 0; j < counts[i]; ++j) {
                        // Add the basevertex offset
                        GLint adjustedIndex = srcIndices[j] + basevertex[i];
                        
                        // Clamp to valid range
                        adjustedIndex = std::max(0, std::min(adjustedIndex, 65535));
                        
                        combinedIndices[offset++] = static_cast<GLushort>(adjustedIndex);
                    }
                    
                    // Add primitive restart index between draws
                    if (i < drawcount - 1) {
                        combinedIndices[offset++] = restartIndex;
                    }
                }
                
                // Upload combined indices
                SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
                OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLushort), 
                           combinedIndices.data(), GL_STREAM_DRAW);
                
                // Draw with primitive restart
                glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                glDrawElements(mode, offset, type, nullptr);
                glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                break;
            }
            
            case GL_UNSIGNED_INT: {
                std::vector<GLuint> combinedIndices(newTotalIndices);
                GLuint offset = 0;
                GLuint restartIndex = 0xFFFFFFFF;
                
                for (GLsizei i = 0; i < drawcount; ++i) {
                    if (counts[i] <= 0) continue;
                    
                    // Get the source indices
                    const GLuint* srcIndices = static_cast<const GLuint*>(indices[i]);
                    
                    // Batch of indices for this draw call
                    for (GLsizei j = 0; j < counts[i]; ++j) {
                        // Add the basevertex offset safely
                        GLint baseVal = basevertex[i];
                        GLuint srcVal = srcIndices[j];
                        
                        // Handle overflow/underflow for negative basevertex
                        GLuint adjustedIndex;
                        if (baseVal < 0 && static_cast<GLuint>(-baseVal) > srcVal) {
                            adjustedIndex = 0; // Clamp to 0 if would underflow
                        } else {
                            adjustedIndex = srcVal + baseVal;
                        }
                        
                        combinedIndices[offset++] = adjustedIndex;
                    }
                    
                    // Add primitive restart index between draws
                    if (i < drawcount - 1) {
                        combinedIndices[offset++] = restartIndex;
                    }
                }
                
                // Upload combined indices
                SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
                OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLuint), 
                           combinedIndices.data(), GL_STREAM_DRAW);
                
                // Draw with primitive restart
                glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                glDrawElements(mode, offset, type, nullptr);
                glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
                break;
            }
            
            default:
                // Unsupported type, fall back to individual calls
                for (GLsizei i = 0; i < drawcount; ++i) {
                    if (counts[i] <= 0) continue;
                    glDrawElementsBaseVertex(mode, counts[i], type, indices[i], basevertex[i]);
                }
                break;
        }
        */
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
