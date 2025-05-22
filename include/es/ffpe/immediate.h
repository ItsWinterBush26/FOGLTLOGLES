#pragma once

#include "es/ffp.h"
#include "es/utils.h"
#include "gles/draw_overrides.h"
#include "gles/ffp/arrays.h"
#include "gles/ffp/enums.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <cstddef>
#include <vector>

namespace FFPE::Rendering::ImmediateMode {

namespace States {
    inline GLenum primitive;

    inline std::vector<decltype(
        FFPE::States::VertexData::VertexRepresentation::position
    )> vertexPositions;

    inline std::vector<decltype(
        FFPE::States::VertexData::VertexRepresentation::color
    )> vertexColors;

    inline std::vector<decltype(
        FFPE::States::VertexData::VertexRepresentation::texCoord
    )> vertexTexCoords;
}

// inine GLuint vbo;

inline void init() {
    // glGenBuffers(1, &vbo);

    // LOGI("immediate mode buffer = %u", vbo);
}

inline void begin(GLenum primitive) {
    if (States::primitive != GL_NONE) {
        LOGE("glBegin has not been ended yet!");
        return;
    }

    LOGI("glBegin()!");

    States::primitive = primitive;
}

inline void advance() {
    LOGI("glVertex*()! advancing!");
    States::vertexPositions.push_back(
        FFPE::States::VertexData::position
    );

    States::vertexColors.push_back(
        FFPE::States::VertexData::color
    );

    States::vertexTexCoords.push_back(
        FFPE::States::VertexData::texCoord
    );
}

inline void end() {
    if (States::primitive == GL_NONE) {
        LOGE("glEnd has not been called yet!");
        return;
    }

    LOGI("glEnd()!");

    if ((States::vertexPositions.size()) == 0) {
        LOGW("glEnd called with no vertices??");
        LOGW("Ignoring!!");
        return;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(
        decltype(FFPE::States::VertexData::VertexRepresentation::position)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                FFPE::States::VertexData::VertexRepresentation::position
            )::value_type
        >::value, 0,
        (void*) States::vertexPositions.data()
    );

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(
        decltype(FFPE::States::VertexData::VertexRepresentation::color)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                FFPE::States::VertexData::VertexRepresentation::color
            )::value_type
        >::value, 0,
        (void*) States::vertexColors.data()
    );

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(
        decltype(FFPE::States::VertexData::VertexRepresentation::texCoord)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                FFPE::States::VertexData::VertexRepresentation::texCoord
            )::value_type
        >::value, 0,
        (void*) States::vertexTexCoords.data()
    );

    Lists::displayListManager->ignoreNextCall();
    OV_glDrawArrays(States::primitive, 0, States::vertexPositions.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    States::primitive = GL_NONE;
    States::vertexPositions.clear();
    States::vertexColors.clear();
    States::vertexTexCoords.clear();

    LOGI("glEnd() [END]!");
}

}
