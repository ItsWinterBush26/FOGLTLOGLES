#pragma once

#include "es/binding_saver.hpp"
#include "es/ffpe/shadergen/main.hpp"
#include "es/ffpe/shadergen/uniforms.hpp"
#include "es/ffpe/states.hpp"
#include "es/ffpe/vao.hpp"
#include "gles20/shader_overrides.hpp"

#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::Arrays {

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
inline GLuint indicesOutputBuffer;
inline GLuint numQuadsUniLoc;

inline void init() {
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

inline GLuint generateEAB_GPU(GLuint count) {
    GLuint quadCount = count / 4;
    GLuint eabCount = quadCount * 6;

    OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);
    OV_glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        eabCount * sizeof(GLuint),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, indicesOutputBuffer);

    SaveUsedProgram sup;
    glUseProgram(eabGeneratorProgram);
    glUniform1ui(numQuadsUniLoc, quadCount);
    
    glDispatchCompute((quadCount + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);

    // OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);

    return eabCount;
}

// https://github.com/MobileGL-Dev/MobileGlues/blob/4902f3a629629ad17ad34165c7eec17a3a2d46b6/src/main/cpp/gl/fpe/fpe.cpp#L31
inline GLuint generateEAB_CPU(GLuint n) {
    GLuint num_quads = n / 4;
    GLuint num_indices = num_quads * 6;

    std::vector<GLuint> indices;
    indices.resize(num_indices);
    
    for (GLuint i = 0; i < num_quads; i++) {
        GLuint base_index = i * 4;

        indices[i * 6 + 0] = base_index + 0;
        indices[i * 6 + 1] = base_index + 1;
        indices[i * 6 + 2] = base_index + 2;

        indices[i * 6 + 3] = base_index + 2;
        indices[i * 6 + 4] = base_index + 3;
        indices[i * 6 + 5] = base_index + 0;
    }

    OV_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);
    OV_glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        num_indices * sizeof(GLuint),
        indices.data(),
        GL_DYNAMIC_DRAW
    );

    return num_indices;
}

inline void drawQuads(GLuint count) {
    LOGI("quads!");
    SaveUsedProgram sup;
    SaveBoundedBuffer sbb(GL_ELEMENT_ARRAY_BUFFER);

    auto buffer = Rendering::VAO::prepareVAOForRendering(count);
    auto renderingProgram = Rendering::ShaderGen::getCachedOrNewProgram(States::Manager::getOrBuildState());
    OV_glUseProgram(renderingProgram);
    Rendering::ShaderGen::Uniforms::setupInputsForRendering(renderingProgram);

    GLuint realCount = generateEAB_GPU(count);

    glDrawElements(GL_TRIANGLES, realCount, GL_UNSIGNED_INT, nullptr);
    LOGI("done quads!");
}

}

inline void init() {
    Quads::init();
    Rendering::VAO::init();
    Rendering::ShaderGen::initFeatures();
}

inline void drawArrays(GLenum mode, GLint first, GLuint count) {
    if (trackedStates->currentlyUsedProgram == 0) {
        // immediately assume FFP

        auto buffer = Rendering::VAO::prepareVAOForRendering(count);
        auto renderingProgram = Rendering::ShaderGen::getCachedOrNewProgram(FFPE::States::Manager::getOrBuildState());
        OV_glUseProgram(renderingProgram);
        Rendering::ShaderGen::Uniforms::setupInputsForRendering(renderingProgram);

        glDrawArrays(mode, first, count);

        OV_glUseProgram(0);
        return;
    }

    glDrawArrays(mode, first, count);
}

}
