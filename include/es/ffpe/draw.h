#pragma once

#include "es/ffp.h"
#include "gles20/shader_overrides.h"

#include <GLES2/gl2.h>
#include <GLES3/gl32.h>
#include <string>

namespace FFPE::Rendering::Arrays {

namespace Attributes {

inline GLuint VBO;

inline void init() {
    glGenVertexArrays(1, &VBO);
}

inline void enableEnabledAttributes() {
    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)

    glBindVertexArray(VBO);
    
    auto vertex = FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
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
}

}
    
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
    count = generateEAB(count);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesOutputBuffer);
    
    FFPE::Rendering::Arrays::Attributes::enableEnabledAttributes();

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}

inline const std::string arrayShaderVS = R"(#version 320 es

layout(location = 0) in vec3 iVertexPosition;
layout(location = 1) in vec4 iVertexColor;
layout(location = 2) in vec2 iVertexTexCoord;

out vec4 vertexColor;
out vec2 vertexTexCoord;

void main() {
    gl_Position = vec4(iVertexPosition, 1.0f);
    oVertexColor = iVertexColor;
    oVertexTexCoord = iVertexTexCoord;
})";

inline const std::string arrayShaderFS = R"(#version 320 es

in vec4 vertexColor;
in vec2 vertexTexCoord;

out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    fragColor = texture(texture0, vertexTexCoord) * vertexColor;
})";

inline GLuint renderingProgram;

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
    
    Attributes::init();
    Quads::init();
}

inline void drawArrays(GLenum mode, GLint first, GLuint count) {
    glUseProgram(renderingProgram);
    
    FFPE::Rendering::Arrays::Attributes::enableEnabledAttributes();
    
    glDrawArrays(mode, first, count);
}

}
