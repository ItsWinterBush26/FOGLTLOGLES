#pragma once

#include "gles/ffp/enums.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

#include <GLES3/gl32.h>

namespace FFPE::States {

namespace VertexData {
    template<int PC, typename P, int CC, typename C, int TCC, typename TC>
    struct VertexRepresentation {
        VertexRepresentation() = default;
        VertexRepresentation(
            glm::vec<PC, P> p, glm::vec<CC, C> c, glm::vec<TCC, TC> tc
        ) : position(p), color(c), texCoord(tc) { }

        glm::vec<PC, P> position = glm::vec4(0, 0, 0, 1);
        glm::vec<CC, C> color = glm::vec4(255, 255, 255, 255);
        glm::vec<TCC, TC> texCoord = glm::vec4(0, 0, 0, 0);
    };

    inline glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    inline glm::vec4 color = glm::vec4(1.0f);
    inline glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
    inline glm::vec4 texCoord = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // TODO: make a <texunit, texcoord> map

    template<typename SRC, typename DST>
    inline void set(SRC src, DST& to) {
        static_assert(
            SRC::length() <= DST::length(),
            "'src' components must be less than or equal to 'dst' components"
        );
        for (size_t i = 0; i < SRC::length(); ++i) to[i] = src[i];
    }
}
namespace AlphaTest {
    inline GLenum op = GL_ALWAYS;
    inline GLclampf threshold = 0.0f;
}
namespace ShadeModel {
    inline GLenum type = GL_SMOOTH;
}
namespace ClientState {
    namespace Arrays {
        struct ArrayParameters {
            bool planar; // 1> = TRUE, !1 = !TRUE

            GLint size;
            GLenum type;
            bool normalized;
            GLsizei stride; // if stride == 0 then stride = size * sizeof(type) else stride = stride
            const void* firstElement;
        };
        
        struct ArrayState {
            bool enabled;
            ArrayParameters parameters;
        };
        
        inline std::unordered_map<GLenum, ArrayState> arrayStates;

        // GL_TEXTUREi, ArrayState
        inline std::unordered_map<GLenum, ArrayState> texCoordArrayStates;

        inline bool isArrayEnabled(GLenum array) {
            return arrayStates[array].enabled;
        }

        inline bool isTexCoordArrayEnabled(GLenum unit) {
            return texCoordArrayStates[unit].enabled;
        }

        inline ArrayState* getArray(GLenum array) {
            return &arrayStates[array];
        }

        inline ArrayState* getTexCoordArray(GLenum unit) {
            return &texCoordArrayStates[unit];
        }
    }

    inline GLenum currentTexCoordUnit;
    inline std::vector<GLenum> texCoordArrayTexUnits;
}

namespace Manager {

namespace States {
    inline bool isDirty = true;

    inline union StateRepresentation {
        struct Fields {
            GLuint64 alphaOp : 4; // 0-7 (8 possible values)
            GLuint64 shadeType : 1; // 0-1 (2 possible values)

            GLuint64 vertexArrayEnabled : 1; // 0-1 (2 possible values)
            GLuint64 vertexArrayComponentSize : 2; // 0-3 (4 possible values)

            GLuint64 colorArrayEnabled : 1; // 0-1 (2 possible values)
            GLuint64 colorArrayComponentSize : 2; // 0-3 (4 possible values)

            GLuint64 texCoordArrayEnabled : 1; // 0-1 (2 possible values)
            GLuint64 texCoordArrayComponentSize : 2; // 0-3 (4 possible values)
        } fields;
        GLbitfield64 value;
    } currentState;
}

inline void markStateAsDirty() {
    States::isDirty = true;
}

inline GLbitfield64 getOrBuildState() {
    if (!States::isDirty) return States::currentState.value;

    States::StateRepresentation result { };

    result.fields.alphaOp = AlphaTest::op - GL_NEVER;
    result.fields.shadeType = ShadeModel::type - GL_FLAT;
    
    auto* vertex = ClientState::Arrays::getArray(GL_VERTEX_ARRAY);
    result.fields.vertexArrayEnabled = vertex->enabled;
    result.fields.vertexArrayComponentSize = vertex->parameters.size - 1;
    
    auto* color = ClientState::Arrays::getArray(GL_COLOR_ARRAY);
    result.fields.colorArrayEnabled = color->enabled;
    result.fields.colorArrayComponentSize = color->parameters.size - 1;

    auto* texCoord = ClientState::Arrays::getTexCoordArray(GL_TEXTURE0);
    result.fields.texCoordArrayEnabled = texCoord->enabled;
    result.fields.texCoordArrayComponentSize = texCoord->parameters.size - 1;
    
    States::isDirty = false;
    States::currentState = result;

    return result.value;
}

}

}