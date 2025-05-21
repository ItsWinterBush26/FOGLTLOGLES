#pragma once


#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/utils.h"
#include "gles/draw_overrides.h"
#include "gles/ffp/arrays.h"
#include "gles20/buffer_tracking.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <cstddef>
#include <vector>

namespace FFPE::Rendering::ImmediateMode {

namespace States {
    inline GLenum primitive;

    inline std::vector<FFPE::States::VertexData::VertexRepresentation> vertices;
}

inline GLuint vbo;

inline void init() {
    glGenBuffers(1, &vbo);
}

inline void begin(GLenum primitive) {
    if (States::primitive != GL_NONE) {
        LOGW("glBegin has not been ended yet!");
        return;
    }

    LOGI("glBegin()!");

    States::primitive = primitive;
}

inline void advance() {
    States::vertices.push_back({
        FFPE::States::VertexData::position,
        FFPE::States::VertexData::color
    });
}

inline void end() {
    if (States::primitive == GL_NONE) {
        LOGI("glBegin has not been called yet!");
        return;
    }

    LOGI("glEnd()!");

    SaveBoundedBuffer sbb(GL_ARRAY_BUFFER);
    OV_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        States::vertices.size() * sizeof(FFPE::States::VertexData::VertexRepresentation),
        States::vertices.data(),
        GL_STATIC_DRAW
    );

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(
        decltype(FFPE::States::VertexData::VertexRepresentation::position)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                FFPE::States::VertexData::VertexRepresentation::position
            )::value_type
        >::value,
        0, (void*) offsetof(
            FFPE::States::VertexData::VertexRepresentation, position
        )
    );

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(
        decltype(FFPE::States::VertexData::VertexRepresentation::color)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                FFPE::States::VertexData::VertexRepresentation::color
            )::value_type
        >::value,
        0,
        (void*) offsetof(
            FFPE::States::VertexData::VertexRepresentation, color
        )
    );

    OV_glDrawArrays(States::primitive, 0, States::vertices.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    States::primitive = GL_NONE;
    States::vertices.clear();
}

}
