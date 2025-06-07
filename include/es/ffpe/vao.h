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

struct VertexData { };

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
    F&& callback
) {
    ESUtils::TypeTraits::dispatchAsType(vertex->parameters.type, [&]<typename POS>() {
        ESUtils::TypeTraits::dispatchAsType(color->parameters.type, [&]<typename COL>() {
            GLsizei newVABSize = count * sizeof(VertexDataTyped<POS, COL>);
            
            OV_glBindBuffer(GL_ARRAY_BUFFER, vab);
            OV_glBufferData(
                GL_ARRAY_BUFFER,
                newVABSize,
                nullptr, GL_STATIC_DRAW
            );

            auto* v = static_cast<
                VertexDataTyped<POS, COL>*
            >(glMapBufferRange(
                GL_ARRAY_BUFFER, 0,
                newVABSize, GL_MAP_WRITE_BIT
            ));

            callback(v);

            if (v) glUnmapBuffer(v);
        });
    });
}

template<typename T1, typename T2>
inline void fillDataComponents(GLuint& offsetTracker, std::span<const T1> src, T2* dst) {
    for (size_t i = 0; i < src.size(); i++) (*dst)[i] = static_cast<typename T2::value_type>(src[i]);
    offsetTracker += src.size();
}

template<typename T>
inline void putVertexDataInternal(GLenum arrayType, GLsizei dataSize, GLuint verticesCount, const T* src, VertexData* dst) {
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

inline void putVertexData(GLenum arrayType, FFPE::States::ClientState::Arrays::ArrayState* array, VertexData* vertices, GLuint verticesCount) {
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

    // TODO: bind gl*Pointer here as VAO's (doing this with no physical keyboard is making me mentally insane, ease send help)
    std::unique_ptr<SaveBoundedBuffer> sbb;
    
    if (trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer == 0) {
        LOGI("not buffered! setup our own vab");
        sbb = std::make_unique<SaveBoundedBuffer>(GL_ARRAY_BUFFER);
        
        mapVertexData(
            count, vertexArray, colorArray,
            [&](auto* vertices) {
                if (vertexArray->enabled) {

                } else {

                }

                if (colorArray->enabled) {

                } else {
                    
                }
            }
        );

        return sbb;
    }
    
    LOGI("buffered! buffer=%u", trackedStates->boundBuffers[GL_ARRAY_BUFFER].buffer);
    LOGI("vertexattribs!");
    
    LOGI("vertices!");
    if (vertexArray->enabled) {
        glEnableVertexAttribArray(0);
        
        if (vertexArray->parameters.buffered) {
            LOGI("buffered!");
            glVertexAttribPointer(
                AttributeLocations::POSITION_LOCATION,
                vertexArray->parameters.size, vertexArray->parameters.type,
                GL_FALSE,
                vertexArray->parameters.stride,
                vertexArray->parameters.firstElement
            );
        } else {
            LOGI("not buffered!");
            putVertexData(GL_VERTEX_ARRAY, vertexArray, vertices, count);
            glVertexAttribPointer(
                AttributeLocations::POSITION_LOCATION,
                decltype(VertexData::position)::length(),
                vertexArray->parameters.type, GL_FALSE,
                sizeof(VertexData),
                (void*) offsetof(VertexData, position)
            );
        }
    } else {
        LOGW("no vertex array?? should we bail?");
    }

    LOGI("colors!");
    if (colorArray->enabled) {
        glEnableVertexAttribArray(AttributeLocations::COLOR_LOCATION);
        if (colorArray->parameters.buffered) {
            LOGI("buffered!");
            glVertexAttribPointer(
                AttributeLocations::COLOR_LOCATION,
                colorArray->parameters.size, colorArray->parameters.type,
                GL_FALSE,
                colorArray->parameters.stride,
                colorArray->parameters.firstElement
            );
        } else {
            LOGI("not buffered!");
            putVertexData(GL_COLOR_ARRAY, colorArray, vertices, count);
            glVertexAttribPointer(
                AttributeLocations::COLOR_LOCATION,
                decltype(VertexData::color)::length(),
                colorArray->parameters.type, GL_FALSE,
                sizeof(VertexData),
                (void*) offsetof(VertexData, color)
            );
        }
    } else {
        LOGI("using global color state!");
        glDisableVertexAttribArray(AttributeLocations::COLOR_LOCATION);
        glVertexAttrib4fv(
            AttributeLocations::COLOR_LOCATION,
            glm::value_ptr(
                FFPE::States::VertexData::color
            )
        );
    }

    LOGI("texcoords!");
    if (texCoordArray->enabled) {
        glEnableVertexAttribArray(AttributeLocations::TEX_COORD_LOCATION);
        if (texCoordArray->parameters.buffered) {
            LOGI("buffered!");
            glVertexAttribPointer(
                AttributeLocations::TEX_COORD_LOCATION,
                texCoordArray->parameters.size, texCoordArray->parameters.type,
                GL_FALSE,
                texCoordArray->parameters.stride,
                texCoordArray->parameters.firstElement
            );
        } else {
            LOGI("not buffered!");
            putVertexData(GL_TEXTURE_COORD_ARRAY, texCoordArray, vertices, count);
            glVertexAttribPointer(
                AttributeLocations::TEX_COORD_LOCATION,
                decltype(VertexData::texCoord)::length(),
                texCoordArray->parameters.type, GL_FALSE,
                sizeof(VertexData),
                (void*) offsetof(VertexData, texCoord)
            );
        }
    } else {
        LOGI("using global texCoord state!");
        glDisableVertexAttribArray(AttributeLocations::TEX_COORD_LOCATION);
        glVertexAttrib4fv(
            AttributeLocations::TEX_COORD_LOCATION,
            glm::value_ptr(
                FFPE::States::VertexData::texCoord
            )
        );
    }

    return sbb;
}

}
