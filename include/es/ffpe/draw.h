#pragma once

#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/ffpe/vao.h"
#include "gles20/shader_overrides.h"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::Arrays {

// TODO: since vecN, where N is the component
// and it might change, we need to...
// SHADERGEN. implement shadergen!
inline const std::string arrayShaderVS = R"(#version 320 es

layout(location = 0) in vec4 iVertexPosition;
layout(location = 1) in vec4 iVertexColor;

out vec4 vertexColor;

void main() {
    gl_Position = iVertexPosition;
    vertexColor = iVertexColor;
})";

inline const std::string arrayShaderFS = R"(#version 320 es
precision mediump float;

in vec4 vertexColor;

out vec4 fragColor;

void main() {
    fragColor = vertexColor;
})";

inline GLuint renderingProgram;

namespace Quads {

// based on:
// https://github.com/MobileGL-Dev/MobileGlues/blob/4902f3a629629ad17ad34165c7eec17a3a2d46b6/src/main/cpp/gl/fpe/fpe.cpp#L31
inline const std::string quadEABGeneratorCS = R"(#version 310 es
layout(local_size_x = 64) in;

layout(std430, binding = 0) writeonly buffer IndexBuffer {
    uint indices[];
};

uniform uint numQuads;

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= uint(numQuads)) return;

    uint baseIndex = i * 4u;
    uint outIndex = i * 6u;

    // first tri (0 1 2)
    indices[outIndex + 0u] = baseIndex + 0u;
    indices[outIndex + 1u] = baseIndex + 1u;
    indices[outIndex + 2u] = baseIndex + 2u;

    // second tri (2 3 0)
    indices[outIndex + 3u] = baseIndex + 2u;
    indices[outIndex + 4u] = baseIndex + 3u;
    indices[outIndex + 5u] = baseIndex + 0u;
})";

inline GLuint eabGeneratorProgram;
inline GLuint countInputBuffer, indicesOutputBuffer;
inline GLuint numQuadsUniLoc;

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

    numQuadsUniLoc = glGetUniformLocation(eabGeneratorProgram, "numQuads");

    glDeleteShader(computeShader);
}

inline GLuint generateEAB(GLuint count) {
    GLuint quadCount = count / 4;
    GLuint eabCount = quadCount * 6;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesOutputBuffer);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        eabCount * sizeof(GLuint),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, indicesOutputBuffer);

    SaveUsedProgram sup;
    glUseProgram(eabGeneratorProgram);
    glUniform1ui(numQuadsUniLoc, quadCount);
    
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
