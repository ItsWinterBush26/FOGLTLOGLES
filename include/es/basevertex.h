#pragma once

#include "es/binding_saver.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl32.h>
#include <vector>
#include <cstring>
#include <memory>

inline GLint getTypeSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:  return 1;
        case GL_UNSIGNED_SHORT: return 2;
        case GL_UNSIGNED_INT:   return 4;
        default:                return 0;
    }
}

template <typename T>
inline void adjustAndCopy(const T* src, T* dst, GLsizei count, int base) {
    for (GLsizei i = 0; i < count; ++i)
        dst[i] = src[i] + static_cast<T>(base);
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

    void batchAndDraw(
        GLenum mode,
        const GLsizei* counts,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        if (!usable) return;
        if (drawcount <= 0) return;
        const GLint typeSize = getTypeSize(type);
        if (typeSize == 0) return;
        
        if (drawcount <= 32) {
            for (GLsizei i = 0; i < drawcount; ++i) {
                if (counts[i] <= 0) continue;
                glDrawElementsBaseVertex(mode, counts[i], type, indices[i], basevertex[i]);
            }
            return;
        }
        
        GLsizei totalIndices = 0;
        for (GLsizei i = 0; i < drawcount; ++i) totalIndices += counts[i];
        
        const GLsizei newTotalIndices = totalIndices + (drawcount - 1);
        GLuint restartIndex = 0;
        switch (type) {
            case GL_UNSIGNED_BYTE:  restartIndex = 0xFF; break;
            case GL_UNSIGNED_SHORT: restartIndex = 0xFFFF; break;
            case GL_UNSIGNED_INT:   restartIndex = 0xFFFFFFFF; break;
            default: return;
        }
        
        std::vector<char> cpuBuffer(newTotalIndices * typeSize);
        GLsizei dstOffset = 0;
        for (GLsizei i = 0; i < drawcount; ++i) {
            const GLsizei counti = counts[i];
            if (counti > 0) {
                const char* src = reinterpret_cast<const char*>(indices[i]);
                switch (type) {
                    case GL_UNSIGNED_BYTE: {
                        const GLubyte* srcIndices = reinterpret_cast<const GLubyte*>(src);
                        GLubyte* dest = reinterpret_cast<GLubyte*>(cpuBuffer.data()) + dstOffset;
                        adjustAndCopy<GLubyte>(srcIndices, dest, counti, basevertex[i]);
                        break;
                    }
                    case GL_UNSIGNED_SHORT: {
                        const GLushort* srcIndices = reinterpret_cast<const GLushort*>(src);
                        GLushort* dest = reinterpret_cast<GLushort*>(cpuBuffer.data()) + (dstOffset / 2);
                        adjustAndCopy<GLushort>(srcIndices, dest, counti, basevertex[i]);
                        break;
                    }
                    case GL_UNSIGNED_INT: {
                        const GLuint* srcIndices = reinterpret_cast<const GLuint*>(src);
                        GLuint* dest = reinterpret_cast<GLuint*>(cpuBuffer.data()) + (dstOffset / 4);
                        adjustAndCopy<GLuint>(srcIndices, dest, counti, basevertex[i]);
                        break;
                    }
                }
                dstOffset += counti * typeSize;
            }
            
            if (i < drawcount - 1) {
                switch (type) {
                    case GL_UNSIGNED_BYTE:
                        reinterpret_cast<GLubyte*>(cpuBuffer.data())[dstOffset] = static_cast<GLubyte>(restartIndex);
                        break;
                    case GL_UNSIGNED_SHORT:
                        reinterpret_cast<GLushort*>(cpuBuffer.data())[(dstOffset / 2)] = static_cast<GLushort>(restartIndex);
                        break;
                    case GL_UNSIGNED_INT:
                        reinterpret_cast<GLuint*>(cpuBuffer.data())[(dstOffset / 4)] = static_cast<GLuint>(restartIndex);
                        break;
                }
                dstOffset += typeSize;
            }
        }
        
        SaveBoundBuffer sbb(GL_COPY_WRITE_BUFFER);
        OV_glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, cpuBuffer.size(), cpuBuffer.data(), GL_STREAM_DRAW);

        SaveBoundBuffer sbb2(GL_ELEMENT_ARRAY_BUFFER);
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        
        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glDrawElements(mode, newTotalIndices, type, (void*)0);
        glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    }
};

std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
