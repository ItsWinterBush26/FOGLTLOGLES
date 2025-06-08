#pragma once

#include "gles/ffp/enums.h"
#define GLM_ENABLE_EXPERIMENTAL

#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/state_tracking.h"
#include "es/utils.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <cstddef>
#include <GLES3/gl32.h>
#include <memory>
#include <span>

template<typename P, typename C>
using VertexDataTyped = FFPE::States::VertexData::VertexRepresentation<P, C>;

namespace FFPE::Rendering::VAO {

namespace AttributeLocations {
    inline const GLuint POSITION_LOCATION = 0;
    inline const GLuint COLOR_LOCATION = 1;
    inline const GLuint TEX_COORD_LOCATION = 2;
}

inline GLuint vao;
inline GLuint vab;

inline GLsizei currentVABSize;

inline void init() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vab);
}

template<typename F>
inline void mapVertexData(
    GLsizei count,
    States::ClientState::Arrays::ArrayState* vertex,
    States::ClientState::Arrays::ArrayState* color,
    const F&& callback
) {
    ESUtils::TypeTraits::dispatchAsType(vertex->parameters.type, [&]<typename POS>() {
        ESUtils::TypeTraits::dispatchAsType(color->parameters.type, [&]<typename COL>() {
            using VertexData = VertexDataTyped<POS, COL>;
            GLsizei newVABSize = count * sizeof(VertexData);
            
            OV_glBindBuffer(GL_ARRAY_BUFFER, vab);
            OV_glBufferData(
                GL_ARRAY_BUFFER,
                newVABSize,
                nullptr, GL_STATIC_DRAW
            );

            auto* v = static_cast<VertexData*>(glMapBufferRange(
                GL_ARRAY_BUFFER, 0,
                newVABSize, GL_MAP_WRITE_BIT
            ));

            callback(v);

            if (v) glUnmapBuffer(GL_ARRAY_BUFFER);
        });
    });
}

template<typename T1, typename T2>
inline void fillDataComponents(GLuint& offsetTracker, std::span<const T1> src, T2* dst) {
    for (size_t i = 0; i < src.size(); i++) (*dst)[i] = src[i];
    offsetTracker += src.size();
}

template<typename T, typename VD>
inline void putVertexDataInternal(GLenum arrayType, GLsizei dataSize, GLuint verticesCount, const T* src, VD* dst) {
    GLuint srcOffset = 0;

    switch (arrayType) {
        case GL_VERTEX_ARRAY:
            for (GLuint i = 0; i < verticesCount; ++i) {
                fillDataComponents(
                    srcOffset,
                    std::span(src + srcOffset, dataSize),
                    &dst[i].position
                );
            }
        break;
            
        case GL_COLOR_ARRAY:
            for (GLuint i = 0; i < verticesCount; ++i) {
                fillDataComponents(
                    srcOffset,
                    std::span(src + srcOffset, dataSize),
                    &dst[i].color
                );
            }
        break;

        case GL_TEXTURE_COORD_ARRAY:
            for (GLuint i = 0; i < verticesCount; ++i) {
                fillDataComponents(
                    srcOffset,
                    std::span(src + srcOffset, dataSize),
                    &dst[i].texCoord
                );
            }
        break;
    }
}

template<typename VD>
inline void putVertexData(GLenum arrayType, FFPE::States::ClientState::Arrays::ArrayState* array, VD* vertices, GLuint verticesCount) {
    LOGI("putVertexData : arrayType=%u", arrayType);
    
    switch (array->parameters.type) {
        case GL_UNSIGNED_BYTE:
            putVertexDataInternal(
                arrayType, array->parameters.size, verticesCount,
                ESUtils::TypeTraits::asTypedArray<GLubyte>(
                    array->parameters.firstElement
                ),
                vertices
            );
        break;

        case GL_SHORT:
            putVertexDataInternal(
                arrayType, array->parameters.size, verticesCount,
                ESUtils::TypeTraits::asTypedArray<GLshort>(
                    array->parameters.firstElement
                ),
                vertices
            );
        break;
    
        case GL_FLOAT:
            putVertexDataInternal(
                arrayType, array->parameters.size, verticesCount,
                ESUtils::TypeTraits::asTypedArray<GLfloat>(
                    array->parameters.firstElement
                ),
                vertices
            );
        break;
    }
}

[[nodiscard]]
inline std::unique_ptr<SaveBoundedBuffer> prepareVAOForRendering(GLsizei count) {
    LOGI("vao setup!");
    glBindVertexArray(vao);

    auto vertexArray = FFPE::States::ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    auto colorArray = FFPE::States::ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    auto texCoordArray = FFPE::States::ClientState::Arrays::getTexCoordArray(GL_TEXTURE0);

    if (vertexArray->enabled) {
        glEnableVertexAttribArray(AttributeLocations::POSITION_LOCATION);
    } else {
        LOGW("no vertex array?? should we bail?");
    }

    if (colorArray->enabled) {
        glEnableVertexAttribArray(AttributeLocations::COLOR_LOCATION);
    } else {
        glDisableVertexAttribArray(AttributeLocations::COLOR_LOCATION);
        LOGI("using global color state!");
        glVertexAttrib4fv(
            AttributeLocations::COLOR_LOCATION,
            glm::value_ptr(
                FFPE::States::VertexData::color
            )
        );
    }

    if (texCoordArray->enabled) {
        glEnableVertexAttribArray(AttributeLocations::TEX_COORD_LOCATION);
    } else {
        glDisableVertexAttribArray(AttributeLocations::TEX_COORD_LOCATION);
        LOGI("using global texCoord state!");
        glVertexAttrib4fv(
            AttributeLocations::TEX_COORD_LOCATION,
            glm::value_ptr(
                FFPE::States::VertexData::texCoord
            )
        );
    }

    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)
    std::unique_ptr<SaveBoundedBuffer> sbb;
    
    if (trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer == 0) {
        LOGI("not buffered! setup our own vab");
        sbb = std::make_unique<SaveBoundedBuffer>(GL_ARRAY_BUFFER);

        LOGI("vertexattribs!");
        mapVertexData(
            count, vertexArray, colorArray,
            [&](auto* vertices) {
                using VertexData = std::remove_pointer_t<decltype(vertices)>;
                LOGI("vertex type is %s", typeid(VertexData).name());

                LOGI("vertices!");
                if (vertexArray->enabled) {
                    putVertexData(GL_VERTEX_ARRAY, vertexArray, vertices, count);
                    glVertexAttribPointer(
                        AttributeLocations::POSITION_LOCATION,
                        decltype(VertexData::position)::length(),
                        vertexArray->parameters.type, GL_FALSE,
                        sizeof(VertexData),
                        (void*) offsetof(VertexData, position)
                    );
                }

                LOGI("colors!");
                if (colorArray->enabled) {
                    putVertexData(GL_COLOR_ARRAY, colorArray, vertices, count);
                    glVertexAttribPointer(
                        AttributeLocations::COLOR_LOCATION,
                        decltype(VertexData::color)::length(),
                        colorArray->parameters.type, GL_FALSE,
                        sizeof(VertexData),
                        (void*) offsetof(VertexData, color)
                    );
                }

                LOGI("texcoords!");
                if (texCoordArray->enabled) {
                    putVertexData(GL_TEXTURE_COORD_ARRAY, texCoordArray, vertices, count);
                    glVertexAttribPointer(
                        AttributeLocations::TEX_COORD_LOCATION,
                        decltype(VertexData::texCoord)::length(),
                        texCoordArray->parameters.type, GL_FALSE,
                        sizeof(VertexData),
                        (void*) offsetof(VertexData, texCoord)
                    );
                }
            }
        );

        return sbb;
    }
    
    LOGI("buffered! buffer=%u", trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer);
    LOGI("vertexattribs!");
    
    LOGI("vertices!");
    if (vertexArray->enabled) {
        glVertexAttribPointer(
            AttributeLocations::POSITION_LOCATION,
            vertexArray->parameters.size, vertexArray->parameters.type,
            GL_FALSE,
            vertexArray->parameters.stride,
            vertexArray->parameters.firstElement
        );
    }

    LOGI("colors!");
    if (colorArray->enabled) {
        glVertexAttribPointer(
            AttributeLocations::COLOR_LOCATION,
            colorArray->parameters.size, colorArray->parameters.type,
            GL_FALSE,
            colorArray->parameters.stride,
            colorArray->parameters.firstElement
        );
    }

    LOGI("texcoords!");
    if (texCoordArray->enabled) {
        glVertexAttribPointer(
            AttributeLocations::TEX_COORD_LOCATION,
            texCoordArray->parameters.size, texCoordArray->parameters.type,
            GL_FALSE,
            texCoordArray->parameters.stride,
            texCoordArray->parameters.firstElement
        );
    }

    return sbb;
}

}
