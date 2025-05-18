#pragma once

#include "es/ffp.h"
#include "gles20/shader_overrides.h"

#include <GLES2/gl2.h>
#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::Arrays {
    
namespace Quads {

inline const std::string quadEABGeneratorCS = R"(#version 310 es
layout(local_size_x = 64) in;

// Number of quads = totalVertices / 4
layout(std430, binding = 0) readonly buffer VertexCount {
    uint numQuads;
};

layout(std430, binding = 1) writeonly buffer IndexBuffer {
    uint indices[];
};

void main() {
    uint quadId = gl_GlobalInvocationID.x;
    if (quadId >= numQuads) return;

    uint base = quadId * 4;
    uint outIndex = quadId * 6;

    // Triangle 1
    indices[outIndex + 0] = base + 0;
    indices[outIndex + 1] = base + 1;
    indices[outIndex + 2] = base + 2;

    // Triangle 2
    indices[outIndex + 3] = base + 0;
    indices[outIndex + 4] = base + 2;
    indices[outIndex + 5] = base + 3;
})";

inline GLuint eabGeneratorProgram;
inline GLuint countInputBuffer, indicesOutputBuffer;

inline void init() {
    glGenBuffers(1, &countInputBuffer);
    glGenBuffers(1, &indicesOutputBuffer);

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    const GLchar* computeShaderSource = quadEABGeneratorCS.c_str();
    OV_glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
    OV_glCompileShader(computeShader);

    eabGeneratorProgram = glCreateProgram();
    glAttachShader(eabGeneratorProgram, computeShader);
    OV_glLinkProgram(eabGeneratorProgram);

    glDeleteShader(eabGeneratorProgram);
}

inline GLuint generateEAB(GLuint count) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, countInputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(count), &count, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, countInputBuffer);

    GLuint eabCount = sizeof(GLuint) * 6 * count;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesOutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, eabCount, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indicesOutputBuffer);

    glUseProgram(eabGeneratorProgram);
    glDispatchCompute((count + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    return eabCount;
}

inline void handleQuads(GLint first, GLuint count) {
    count = generateEAB(count);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);

    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)

    States::ClientState::Arrays::ArrayState vertex = FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    if (vertex.enabled) {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            vertex.parameters.size,
            vertex.parameters.type,
            GL_FALSE,
            vertex.parameters.stride,
            (void*) vertex.parameters.offset
        );
    }

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}

}
