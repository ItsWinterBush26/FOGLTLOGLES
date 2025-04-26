#pragma once

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"

#include <GLES3/gl32.h>
#include <memory>
#include <string>
#include <vector>

// Some of the code comes from:
// https://github.com/MobileGL-Dev/MobileGlues/blob/8727ed43fde193ae595d73e84a8991ee771e43e7/src/main/cpp/gl/multidraw.cpp#L418

inline const std::string COMPUTE_BATCHER_GLSL_BASE = R"(
#version 320 es
layout(local_size_x = 128) in;

struct DrawCommand {
    uint  firstIndex;
    int   baseVertex;
};

layout(std430, binding = 0) readonly buffer Input {
    uint inputElementBuffer[];
};

layout(std430, binding = 1) readonly buffer DrawCommands {
    DrawCommand drawCommands[];
};

layout(std430, binding = 2) readonly buffer PrefixSummations {
    uint prefixSums[];
};

layout(std430, binding = 3) writeonly buffer Output {
    uint outputIndices[];
};

void main() {
    uint outputIndex = gl_GlobalInvocationID.x;
    uint total = prefixSums[prefixSums.length() - 1];
    if (outputIndex >= total) return;

    int low = 0, high = prefixSums.length();
    while (low < high) {
        int mid = (low + high) >> 1;
        if (prefixSums[mid] <= outputIndex) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    DrawCommand cmd = drawCommands[low];

    uint prevSum = low > 0 ? prefixSums[low - 1] : 0u;
    uint localIndex = outputIndex - prevSum;
    uint inputIndex = localIdx + cmd.firstIndex;

    outputIndices[outputIndex] = inputElementBuffer[inputIndex] + uint(cmd.baseVertex);
}
)";

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

    GLuint paramsSSBO;
    GLuint prefixSSBO;
    GLuint outputIndexSSBO;

    bool computeReady;

    MDElementsBaseVertexBatcher() {
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

        const GLchar* castedSource = COMPUTE_BATCHER_GLSL_BASE.c_str();
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
        glGenBuffers(1, &prefixSSBO);
        glGenBuffers(1, &outputIndexSSBO);
    }

    ~MDElementsBaseVertexBatcher() {
        glDeleteProgram(computeProgram);
        glDeleteBuffers(1, &paramsSSBO);
        glDeleteBuffers(1, &prefixSSBO);
        glDeleteBuffers(1, &outputIndexSSBO);
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
        if (drawcount <= 0 || elemSize != 4) return; // force GL_UINT support for now

        /* if (!computeReady || drawcount < 1024) {
            for (GLint i = 0; i < drawcount; ++i) {
                if (count[i] > 0) glDrawElementsBaseVertex(mode, count[i], type, indices[i], basevertex[i]);
            }
        } */
        
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, paramsSSBO);
        const GLintptr previousSSBOSize = trackedStates->boundBuffers[GL_SHADER_STORAGE_BUFFER].size;
        const long drawCommandsSize = static_cast<long>(drawcount * sizeof(DrawCommand));
        if (previousSSBOSize < drawCommandsSize) {
            OV_glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                drawCommandsSize,
                nullptr, GL_DYNAMIC_DRAW
            );
        }

        DrawCommand* drawCommands = reinterpret_cast<DrawCommand*>(
            glMapBufferRange(
                GL_SHADER_STORAGE_BUFFER,
                0,
                drawCommandsSize,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
            )
        );

        for (GLsizei i = 0; i < drawcount; ++i) {
            size_t byteOffset = reinterpret_cast<uintptr_t>(indices[i]);
            drawCommands[i].firstIndex = static_cast<GLuint>(byteOffset >> 2); // >> 2 -> / 4
            drawCommands[i].baseVertex = basevertex ? basevertex[i] : 0;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        std::vector<GLuint> prefix(drawcount);
        prefix[0] = count[0];
        for (int i = 1; i < drawcount; ++i) prefix[i] = prefix[i - 1] + count[i];
        GLuint total = prefix.back();
        
        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, prefixSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            prefix.size() * sizeof(GLuint),
            prefix.data(), GL_DYNAMIC_DRAW
        );

        OV_glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputIndexSSBO);
        OV_glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            total * sizeof(GLuint),
            nullptr, GL_DYNAMIC_DRAW
        );

        SaveUsedProgram sup;
        glUseProgram(computeProgram);
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 0,
            trackedStates->boundBuffers[GL_ELEMENT_ARRAY_BUFFER].buffer
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 1,
            paramsSSBO
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 2,
            prefixSSBO
        );
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER, 3,
            outputIndexSSBO
        );

        glDispatchCompute((total + 127) / 128, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);
        OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outputIndexSSBO);
        glDrawElements(mode, total, type, 0);
    }
};

inline std::shared_ptr<MDElementsBaseVertexBatcher> batcher;
