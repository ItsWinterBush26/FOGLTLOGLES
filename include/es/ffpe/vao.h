#pragma once

#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/state_tracking.h"
#include "gles20/buffer_tracking.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include <cstddef>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <GLES3/gl32.h>

namespace FFPE::Rendering::VAO {

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texCoord;
};

inline GLuint vao;
inline GLuint vab;

inline GLsizei currentVABSize;

inline void init() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vab);
}

inline void resizeVAB(GLsizei count) {
    currentVABSize = count * sizeof(VertexData);

    glBindBuffer(GL_ARRAY_BUFFER, vab);
    glBufferData(
        GL_ARRAY_BUFFER,
        currentVABSize,
        nullptr, GL_STATIC_DRAW
    );
}

inline void putVertexData(GLenum arrayType, const void* array) {
    LOGI("putVertexData : arrayType=%u", arrayType);
    OV_glBindBuffer(GL_ARRAY_BUFFER, vab);
    
    // TODO: support other types
    const GLfloat* dataPoints = (const GLfloat*) array;
    VertexData* vertices = (VertexData*) glMapBufferRange(
        GL_ARRAY_BUFFER, 0,
        currentVABSize, GL_MAP_WRITE_BIT
    );

    GLsizei verticesCount = currentVABSize / sizeof(VertexData);
    GLsizei dataPointsCursor = 0;
    for (GLsizei i = 0; i < verticesCount; ++i) {
        switch (arrayType) {
            case GL_VERTEX_ARRAY: {
                glm::vec3 position = glm::vec3(
                    dataPoints[dataPointsCursor],
                    dataPoints[dataPointsCursor + 1],
                    dataPoints[dataPointsCursor + 2]
                );

                LOGI("vertices[%i].pos = %s", i, glm::to_string(position).c_str());

                vertices[i].position = position;
                dataPointsCursor += 3;
            }
            break;
            case GL_COLOR_ARRAY: {
                glm::vec4 color = glm::vec4(
                    dataPoints[dataPointsCursor],
                    dataPoints[dataPointsCursor + 1],
                    dataPoints[dataPointsCursor + 2],
                    dataPoints[dataPointsCursor + 3]
                );

                LOGI("vertices[%i].col = %s", i, glm::to_string(color).c_str());

                vertices[i].color = color;
                dataPointsCursor += 4;
            }
            break;
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

inline void prepareVAOForRendering(GLsizei count) {
    LOGI("vao setup!");
    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)

    SaveBoundedBuffer* sbb = nullptr;
    if (trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer == 0) {
        LOGI("not buffered!");
        sbb = new SaveBoundedBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, vab);
        if ((count * sizeof(VertexData)) > (unsigned long) currentVABSize) {
            resizeVAB(count);
        }
    }

    glBindVertexArray(vao);

    auto vertexArray = FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    if (vertexArray->enabled) {
        glEnableVertexAttribArray(0);
        
        if (vertexArray->parameters.buffered) {
            glVertexAttribPointer(
                0, 3, vertexArray->parameters.type, GL_FALSE,
                vertexArray->parameters.stride, vertexArray->parameters.firstElement
            );
        } else {
            putVertexData(GL_VERTEX_ARRAY, vertexArray->parameters.firstElement);
            glVertexAttribPointer(
                0, 3, vertexArray->parameters.type, GL_FALSE,
                vertexArray->parameters.stride, (void*) offsetof(VertexData, position)
            );
        }
    }
    
    auto colorArray = FFPE::States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    if (colorArray->enabled) {
        glEnableVertexAttribArray(1);
        
        if (colorArray->parameters.buffered) {
            glVertexAttribPointer(
                1, 4, colorArray->parameters.type, GL_FALSE,
               colorArray->parameters.stride, colorArray->parameters.firstElement
            );
        } else {
            putVertexData(GL_COLOR_ARRAY, colorArray->parameters.firstElement);
            glVertexAttribPointer(
                1, 4, colorArray->parameters.type, GL_FALSE,
                colorArray->parameters.stride, (void*) offsetof(VertexData, color)
            );
        }
    }

    if (sbb) delete sbb;
}

}
