#pragma once

#include "es/binding_saver.h"
#include "es/ffpe/vao.h"
#include "gles20/shader_overrides.h"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::Arrays {

inline const std::string arrayShaderVS = R"(#version 320 es

layout(location = 0) in vec3 iVertexPosition;
layout(location = 1) in vec4 iVertexColor;

out vec4 vertexColor;
out vec2 vertexTexCoord;

void main() {
    gl_Position = vec4(iVertexPosition, 1.0f);
    vertexColor = iVertexColor;
    vertexTexCoord = vec2(0.0f, 0.0f); // iVertexTexCoord;
})";

inline const std::string arrayShaderFS = R"(#version 320 es
precision mediump float;

in vec4 vertexColor;
in vec2 vertexTexCoord;

out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    fragColor = /* texture(texture0, vertexTexCoord) */ vertexColor;
})";

inline GLuint renderingProgram;

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

    uint base = quadId * 4u;
    uint outIndex = quadId * 6u;

    // Triangle 1
    indices[outIndex + 0u] = base + 0u;
    indices[outIndex + 1u] = base + 1u;
    indices[outIndex + 2u] = base + 2u;

    // Triangle 2
    indices[outIndex + 3u] = base + 0u;
    indices[outIndex + 4u] = base + 2u;
    indices[outIndex + 5u] = base + 3u;
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

    glDeleteShader(computeShader);
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
    // no checks because GL_QUADS has been deprecated and is only used by old apps (guaranteed to be FFP)
    
    glUseProgram(renderingProgram);
    auto buffer = FFPE::Rendering::VAO::prepareVAOForRendering(count);
    
    count = generateEAB(count);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}

inline void init() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSource = arrayShaderVS.c_str();
    OV_glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSource = arrayShaderFS.c_str();
    OV_glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    
    OV_glCompileShader(vertexShader);
    OV_glCompileShader(fragmentShader);

    renderingProgram = glCreateProgram();
    glAttachShader(renderingProgram, vertexShader);
    glAttachShader(renderingProgram, fragmentShader);
    
    OV_glLinkProgram(renderingProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    Quads::init();
    FFPE::Rendering::VAO::init();
}

inline void drawArrays(GLenum mode, GLint first, GLuint count) {
    if (trackedStates->currentlyUsedProgram == 0) {
        SaveUsedProgram sup;

        glUseProgram(renderingProgram);

        // immediately assume we can do VAO's
        auto buffer = FFPE::Rendering::VAO::prepareVAOForRendering(count);
        
        glDrawArrays(mode, first, count);
        return;
    }

    glDrawArrays(mode, first, count);
}

}
