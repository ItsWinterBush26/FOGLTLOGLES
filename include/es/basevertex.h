#pragma once
#pragma clang optimize off

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "gles20/shader_overrides.h"

#include <GLES3/gl32.h>
#include <memory>
#include <string>
#include <vector>

// Some of the code comes from:
// https://github.com/MobileGL-Dev/MobileGlues/blob/8727ed43fde193ae595d73e84a8991ee771e43e7/src/main/cpp/gl/multidraw.cpp#L418

inline const std::string COMPUTE_BATCHER_GLSL_BASE = R"(#version 320 es
layout(local_size_x = 64) in;

layout(std430, binding = 0) readonly buffer Input {
    uint inputElementBuffer[];
};

layout(std430, binding = 1) readonly buffer Indices {
    uint indices[];
};

layout(std430, binding = 2) readonly buffer BaseVertices {
    int baseVertices[];
};

layout(std430, binding = 3) readonly buffer PrefixSums {
    uint prefixSums[];
};

layout(std430, binding = 4) writeonly buffer Output {
    uint outputIndices[];
};

void main() {
    uint outputIndex = gl_GlobalInvocationID.x;
    if (outputIndex >= prefixSums[prefixSums.length() - 1]) return;

    int low = 0, high = prefixSums.length() - 1;
    while (low < high) {
        int mid = (low + high) >> 1;
        if (prefixSums[mid] > outputIndex) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    uint firstIndex = indices[low] / uint(4);
    int baseVertex = baseVertices[low];

    uint localIndex = outputIndex - ((low == 0) ? 0u : (prefixSums[low - 1]));
    uint inputIndex = localIndex + firstIndex;
    
    outputIndices[outputIndex] = uint(int(inputElementBuffer[inputIndex]) + baseVertex);
})";

struct DrawCommand {
    GLuint firstIndex;
    GLint baseVertex;
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

    GLuint indicesSSBO;
    GLuint baseVerticesSSBO;
    GLuint prefixSSBO;
    GLuint outputIndexSSBO;

    ~MDElementsBaseVertexBatcher() {
        glDeleteProgram(computeProgram);
        glDeleteBuffers(1, &indicesSSBO);
        glDeleteBuffers(1, &baseVerticesSSBO);
        glDeleteBuffers(1, &prefixSSBO);
        glDeleteBuffers(1, &outputIndexSSBO);
    }

    void init() {
        computeProgram = glCreateProgram();
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
        
        const GLchar* castedSource = COMPUTE_BATCHER_GLSL_BASE.c_str();
        OV_glShaderSource(computeShader, 1, &castedSource, nullptr);
        OV_glCompileShader(computeShader);

        glAttachShader(computeProgram, computeShader);
        OV_glLinkProgram(computeProgram);

        glDeleteShader(computeShader);

        glGenBuffers(1, &indicesSSBO);
        glGenBuffers(1, &baseVerticesSSBO);
        glGenBuffers(1, &prefixSSBO);
        glGenBuffers(1, &outputIndexSSBO);
    }

    void batch(
        GLenum mode,
        const GLsizei* counts,
        GLenum type,
        const void* const* indices,
        GLsizei drawcount,
        const GLint* basevertex
    ) {
        const GLuint elemSize = getTypeByteSize(type);
        if (drawcount <= 0 || elemSize != 4) return; // force GL_UINT support for now

        /* if (!computeReady || drawcount < 1024) {
            for (GLint i = 0; i < drawcount; ++i) {
                if (count[i] > 0) glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
            }
        } */

        LOGI("batch begin! drawcount=%i", drawcount);

        LOGI("prepare inputs and output of compute");
        
        std::vector<GLuint> prefix(drawcount);
        prefix[0] = counts[0];
        for (GLsizei i = 1; i < drawcount; ++i) prefix[i] = prefix[i - 1] + counts[i];
        GLuint total = prefix.back();

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            drawcount * elemSize,
            indices, GL_DYNAMIC_DRAW
        );

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, baseVerticesSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            drawcount * sizeof(GLint),
            basevertex, GL_DYNAMIC_DRAW
        );
        
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, prefixSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            drawcount * sizeof(GLuint),
            prefix.data(), GL_DYNAMIC_DRAW
        );

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputIndexSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            total * sizeof(GLuint),
            nullptr, GL_DYNAMIC_DRAW
        );

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        SaveBoundedBuffer sbb2(GL_ELEMENT_ARRAY_BUFFER);
        
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 0, sbb2.boundedBuffer
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 1, indicesSSBO
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 2, baseVerticesSSBO
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 3, prefixSSBO
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 4, outputIndexSSBO
        );

        LOGI("dispatch compute! jobs=%u", (total + 63) / 64);

        SaveBoundedBuffer sbb3(GL_ARRAY_BUFFER);
        SaveUsedProgram sup;
        OV_glUseProgram(computeProgram);
        glDispatchCompute((total + 63) / 64, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        LOGI("restore states");
        
        sup.restore();
        sbb3.restore();
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outputIndexSSBO);

        LOGI("DRAW!");
        glDrawElements(mode, total, type, 0);

        sbb2.restore();
        LOGI("done");
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher = std::make_shared<MDElementsBaseVertexBatcher>();
#pragma clang optimize on
