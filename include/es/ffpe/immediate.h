#pragma once

#include "es/binding_saver.h"
#include "es/ffp.h"
#include "es/utils.h"
#include "gles/draw_overrides.h"
#include "gles/ffp/arrays.h"
#include "gles/ffp/enums.h"
#include "gles20/buffer_tracking.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <cstddef>
#include <vector>

using VertexData = FFPE::States::VertexData::VertexRepresentation<GLfloat, GLfloat>;

namespace FFPE::Rendering::ImmediateMode {

namespace States {
    inline GLenum primitive;

    inline std::vector<VertexData> vertices;
}

inline GLuint vbo;

inline void init() {
    glGenBuffers(1, &vbo);

    LOGI("immediate mode buffer = %u", vbo);
}

inline bool isActive() {
    return States::primitive != GL_NONE;
}

inline void begin(GLenum primitive) {
    if (isActive()) {
        LOGE("glBegin has not been ended yet!");
        return;
    }

    switch (primitive) {
        case GL_POINTS:
        case GL_LINES:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_TRIANGLES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case 0x7: // GL_QUADS
        case 0x8: // GL_QUAD_STRIP
        case 0x9: // GL_POLYGON
            break;
        default:
            return;
    }

    Lists::displayListManager->addCommand<begin>(primitive);

    LOGI("glBegin()!");

    States::primitive = primitive;
}

inline void advance() {
    LOGI("glVertex*()! advancing!");
    States::vertices.emplace_back(
        FFPE::States::VertexData::position,
        FFPE::States::VertexData::color,
        FFPE::States::VertexData::texCoord
    );
}

inline void endInternal(
    const std::vector<VertexData>& vertices
) {
    if (!isActive()) {
        LOGE("glEnd has not been called yet!");
        return;
    }

    LOGI("glEnd()!");

    if (vertices.size() == 0) {
        LOGW("glEnd called with no vertices??");
        LOGW("Ignoring!!");
        return;
    }

    Lists::displayListManager->addCommand<endInternal>(
        std::vector<VertexData>(vertices)
    );

    SaveBoundedBuffer sbb(GL_ARRAY_BUFFER);
    OV_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    OV_glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(VertexData),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(
        decltype(VertexData::position)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                VertexData::position
            )::value_type
        >::value,
        sizeof(VertexData), (void*) offsetof(
            VertexData, position
        )
    );

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(
        decltype(VertexData::color)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                VertexData::color
            )::value_type
        >::value,
        sizeof(VertexData),
        (void*) offsetof(
            VertexData, color
        )
    );

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(
        decltype(VertexData::texCoord)::length(),
        ESUtils::TypeTraits::GLTypeEnum<
            decltype(
                VertexData::texCoord
            )::value_type
        >::value,
        sizeof(VertexData),
        (void*) offsetof(
            VertexData, texCoord
        )
    );

    Lists::displayListManager->ignoreNextCall();
    OV_glDrawArrays(States::primitive, 0, States::vertices.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    States::primitive = GL_NONE;
    States::vertices.clear();

    LOGI("glEnd() [END]!");
}

inline void end() {
    endInternal(States::vertices);
}

}
