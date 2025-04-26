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

struct DrawParam {
    uint firstIndexOffset;
    uint indexCount;
    uint baseVertex;
    uint outputOffset;
};

layout(std430, binding = 0) buffer DrawParams {
    DrawParam params[];
};

layout(std430, binding = 1) buffer InputIndices { uint indices[]; };
layout(std430, binding = 2) buffer OutputIndices { uint outIndices[]; };

void main() {
    uint drawID  = gl_WorkGroupID.x;
    uint localID = gl_LocalInvocationID.x;

    DrawParam param = params[drawID];
    if (localID >= param.indexCount) return;

    uint inIdx  = param.firstIndexOffset + localID;
    uint outIdx = param.outputOffset + localID;
    outIndices[outIdx] = indices[inIdx] + param.baseVertex;
}
)";

struct DrawParam {
    GLuint firstIndexOffset;
    GLuint indexCount;
    GLuint baseVertex;
    GLuint outputOffset;
};

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

    std::vector<DrawParam> drawParams;

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

        drawParams.resize(drawcount);
        GLuint totalIndexCount = 0, maxIndicesPerDraw = 0;
        
        #pragma omp parallel for reduction(+:totalIndexCount) reduction(max:maxIndicesPerDraw)
        for (int i = 0; i < drawcount; ++i) {
            GLuint offset = reinterpret_cast<GLintptr>(indices[i]) / elemSize;
            GLuint cnt = count[i];
            GLuint base = basevertex[i];

            drawParams[i] = {
                offset, cnt, base, 0
            };

            maxIndicesPerDraw = std::max(maxIndicesPerDraw, cnt);
        }

        // Prefix sum
        for (int i = 0; i < drawcount; ++i) {
            drawParams[i].outputOffset = totalIndexCount;
            totalIndexCount += drawParams[i].indexCount;
        }

        // Upload draw params
        SaveBoundedBuffer saveSSBO(GL_SHADER_STORAGE_BUFFER);
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, paramsSSBO);
        OV_glBufferData(GL_SHADER_STORAGE_BUFFER, drawcount * sizeof(DrawParam), drawParams.data(), GL_STATIC_DRAW);

        SaveBoundedBuffer saveEBO(GL_ELEMENT_ARRAY_BUFFER);
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputIdxSSBO);
        OV_glBufferData(GL_SHADER_STORAGE_BUFFER, totalIndexCount * elemSize, nullptr, GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, paramsSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, trackedStates->boundBuffers[GL_ELEMENT_ARRAY_BUFFER].buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, outputIdxSSBO);

        // main
        SaveUsedProgram saveProgram;
        glUseProgram(computeProgram);
        glDispatchCompute(drawcount, 2, 2);
        glMemoryBarrier(GL_ELEMENT_ARRAY_BARRIER_BIT);

        // draw
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outputIdxSSBO);
        glDrawElements(mode, totalIndexCount, type, nullptr);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
