#pragma once

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl32.h>
#include <memory>
#include <string>
#include <vector>

inline const std::string COMPUTE_BATCHER_GLSL = R"(
#version 320 es
layout(local_size_x = 128) in;

layout(std430, binding = 0) buffer DrawParams {
    uint firstIndexOffset[];
    uint indexCount[];
    uint baseVertex[];
    uint outputOffset[];
};

layout(std430, binding = 1) buffer InputIndices { uint indices[]; };
layout(std430, binding = 2) buffer OutputIndices { uint outIndices[]; };

void main() {
    uint drawID  = gl_WorkGroupID.x;
    uint localID = gl_LocalInvocationID.x;
    uint cnt     = indexCount[drawID];
    if (localID >= cnt) return;

    uint inIdx  = firstIndexOffset[drawID] + localID;
    uint bv     = baseVertex[drawID];
    uint outIdx = outputOffset[drawID] + localID;

    outIndices.outIndices[outIdx] = indices[inIdx] + bv;
}
)";

inline GLuint getTypeByteSize(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE: return 1;
        case GL_UNSIGNED_SHORT: return 2;
        case GL_UNSIGNED_INT: return 4;
        default: return 0;
    }
}

struct MDElementsBaseVertexBatcher {
    GLuint computeProgram;

    GLuint paramsSSBO;
    GLuint outputIdxSSBO;

    bool computeReady;

    MDElementsBaseVertexBatcher() {
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

        const GLchar* castedSource = COMPUTE_BATCHER_GLSL.c_str();
        glShaderSource(computeShader, 1, &castedSource, nullptr);
        glCompileShader(computeShader);

        GLint success = 0;
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);

        computeProgram = glCreateProgram();
        glAttachShader(computeProgram, computeShader);
        glLinkProgram(computeProgram);

        GLint success2 = 0;
        glGetProgramiv(computeProgram, GL_LINK_STATUS, &success2);

        glDeleteShader(computeShader);

        LOGI("MDElementsBaseVertexBatcher.computeReady=%s", (success && success2) ? "true" : "false");
        if (!(success && success2)) return;

        glGenBuffers(1, &paramsSSBO);
        glGenBuffers(1, &outputIdxSSBO);


    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteProgram(computeProgram);
        glDeleteBuffers(1, &paramsSSBO);
        glDeleteBuffers(1, &outputIdxSSBO);
    }

    void batch(
        GLenum mode,
        const GLsizei* count,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        const GLuint elemSize = getTypeByteSize(type);
        if (drawcount <= 0 || elemSize == 0) return;

        /* if (!computeReady || drawcount < 1024) {
            for (GLint i = 0; i < drawcount; ++i) {
                if (count[i] > 0) glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
            }
        } */

        std::vector<GLuint> firstIndexOffset(drawcount),
                            indexCount(drawcount),
                            baseVertex(drawcount),
                            outputOffset(drawcount);
        
        // omp this
        GLuint totalIndexCount = 0, maxIndicesPerDraw = 0;
        for (int i = 0; i < drawcount; ++i) {
            firstIndexOffset[i] = reinterpret_cast<GLintptr>(indices[i]) / elemSize;
            indexCount[i]       = count[i];
            baseVertex[i]       = basevertex[i];
            outputOffset[i]     = totalIndexCount;
            totalIndexCount    += count[i];
            maxIndicesPerDraw   = std::max<GLuint>(maxIndicesPerDraw, count[i]);
        }

        SaveBoundedBuffer sbb(GL_SHADER_STORAGE_BUFFER);
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, paramsSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            drawcount * 4 * elemSize, // 4 arrays
            nullptr,
            GL_STATIC_DRAW
        );
        glBufferSubData(
            GL_SHADER_STORAGE_BUFFER, 0,
            drawcount * elemSize, firstIndexOffset.data()
        );
        glBufferSubData(
            GL_SHADER_STORAGE_BUFFER, drawcount * elemSize,
            drawcount * elemSize, indexCount.data()
        );
        glBufferSubData(
            GL_SHADER_STORAGE_BUFFER, 2 * drawcount * elemSize,
            drawcount * elemSize, baseVertex.data()
        );
        glBufferSubData(
            GL_SHADER_STORAGE_BUFFER, 3 * drawcount * elemSize,
            drawcount * elemSize, outputOffset.data()
        );

        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 0, paramsSSBO
        );

        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 1, trackedStates->boundBuffers[GL_ELEMENT_ARRAY_BUFFER].buffer
        ); 

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputIdxSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            totalIndexCount * elemSize,
            nullptr, GL_DYNAMIC_DRAW
        );
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, outputIdxSSBO);

        // main
        SaveUsedProgram sup;
        glUseProgram(computeProgram);

        GLuint groupsY = (maxIndicesPerDraw + 127) / 128;
        glDispatchCompute(drawcount, groupsY, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        SaveBoundedBuffer sbb2(GL_ELEMENT_ARRAY_BUFFER);
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outputIdxSSBO);
        glDrawElements(mode, totalIndexCount, type, nullptr);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
