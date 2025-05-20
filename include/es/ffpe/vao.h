#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/state_tracking.h"
#include "es/utils.h"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtx/string_cast.hpp"
#include "utils/span.h"

#include <cstddef>
#include <GLES3/gl32.h>
#include <memory>

namespace FFPE::Rendering::VAO {

struct VertexData {
    glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

template<typename T1, typename T2>
inline void fillDataComponents(GLsizei& offsetTracker, tcb::span<const T1> src, T2* dst) {
    for (size_t i = 0; src.size(); i++) (*dst)[i] = src[i];
    offsetTracker += src.size();
}

template<typename T>
inline void putVertexDataInternal(GLenum arrayType, GLsizei dataSize, const T* src, VertexData* dst) {
    GLsizei verticesCount = currentVABSize / sizeof(VertexData);
    GLsizei srcOffset = 0;

    for (GLsizei i = 0; i < verticesCount; ++i) {
        switch (arrayType) {
            case GL_VERTEX_ARRAY:
                fillDataComponents(
                    srcOffset,
                    tcb::span(src + srcOffset, dataSize),
                    &dst[i].position
                );
            break;
            
            case GL_COLOR_ARRAY:
                fillDataComponents(
                    srcOffset,
                    tcb::span(src + srcOffset, dataSize),
                    &dst[i].color
                );
            break;
        }
    }
}



inline void putVertexData(GLenum arrayType, VertexData* vertices, FFPE::States::ClientState::Arrays::ArrayState* array) {
    LOGI("putVertexData : arrayType=%u", arrayType);
    
    switch (array->parameters.type) {
        case GL_SHORT:
            putVertexDataInternal(
                arrayType, array->parameters.size,
                ESUtils::TypeTraits::asTypedArray<GLshort>(
                    array->parameters.firstElement
                ),
                vertices
            );
        break;
    
        case GL_FLOAT:
            putVertexDataInternal(
                arrayType, array->parameters.size,
                ESUtils::TypeTraits::asTypedArray<GLfloat>(
                    array->parameters.firstElement
                ),
                vertices
            );
        break;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

[[nodiscard]]
inline std::unique_ptr<SaveBoundedBuffer> prepareVAOForRendering(GLsizei count) {
    LOGI("vao setup!");
    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)
    std::unique_ptr<SaveBoundedBuffer> sbb;

    VertexData* vertices = nullptr;
    if (trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer == 0) {
        LOGI("not buffered!");
        sbb = std::make_unique<SaveBoundedBuffer>(GL_ARRAY_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, vab);
        if ((count * sizeof(VertexData)) > (unsigned long) currentVABSize) {
            resizeVAB(count);
        }

        vertices = (VertexData*) glMapBufferRange(
            GL_ARRAY_BUFFER, 0,
            currentVABSize, GL_MAP_WRITE_BIT
        );
    }

    glBindVertexArray(vao);

    auto vertexArray = FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    if (vertexArray->enabled) {
        glEnableVertexAttribArray(0);
        
        if (vertexArray->parameters.buffered) {
            glVertexAttribPointer(
                0,
                vertexArray->parameters.size, vertexArray->parameters.type,
                GL_FALSE,
                vertexArray->parameters.stride,
                vertexArray->parameters.firstElement
            );
        } else {
            putVertexData(GL_VERTEX_ARRAY, vertices, vertexArray);
            glVertexAttribPointer(
                0, decltype(VertexData::position)::length(),
                vertexArray->parameters.type, GL_FALSE,
                sizeof(VertexData), (void*) offsetof(VertexData, position)
            );
        }
    }
    
    auto colorArray = FFPE::States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    glEnableVertexAttribArray(1);
    if (colorArray->enabled) {
        if (colorArray->parameters.buffered) {
            glVertexAttribPointer(
                1,
                colorArray->parameters.size, colorArray->parameters.type,
                GL_FALSE,
                colorArray->parameters.stride,
                colorArray->parameters.firstElement
            );
        } else {
            putVertexData(GL_COLOR_ARRAY, vertices, colorArray);
            glVertexAttribPointer(
                1, decltype(VertexData::color)::length(),
                colorArray->parameters.type, GL_FALSE,
                sizeof(VertexData), (void*) offsetof(VertexData, color)
            );
        }
    } else {
        std::vector<glm::vec4> colors(count, FFPE::States::VertexData::color);
        putVertexDataInternal(
            GL_COLOR_ARRAY, FFPE::States::VertexData::color.length(),
            ESUtils::TypeTraits::asTypedArray<GLshort>(colors.data()), vertices
        );
        glVertexAttribPointer(
            1, sizeof(VertexData::color),
            GL_FLOAT, GL_FALSE,
            sizeof(VertexData), (void*) offsetof(VertexData, color)
        );
    }

    if (vertices) glUnmapBuffer(GL_ARRAY_BUFFER);
    return sbb;
}

}
