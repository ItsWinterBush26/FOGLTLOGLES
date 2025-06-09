#pragma once

#include "gles/ffp/enums.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "utils/conversions.hpp"
#include "utils/fast_map.hpp"

#include <cstddef>
#include <GLES3/gl32.h>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

// TODO: break out and move to specific files

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

    template<typename VT, typename CT>
    inline void set(CT val, VT* dst) {
        static_assert(
            decltype(val)::length() <= std::remove_reference_t<decltype(*dst)>::length(),
            "'src' components must only be less than or equals of 'dst' components"
        );
        for (size_t i = 0; i < decltype(val)::length(); ++i) (*dst)[i] = val[i];
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

// TODO: incremental hashing
inline GLbitfield buildCurrentStatesBitfield() {
    return (
        AlphaTest::op | float_to_bits(AlphaTest::threshold) |
        ShadeModel::type |
        (
            ClientState::Arrays::getArray(GL_VERTEX_ARRAY)->parameters.size *
            ClientState::Arrays::getArray(GL_COLOR_ARRAY)->parameters.size *
            ClientState::Arrays::getTexCoordArray(GL_TEXTURE0)->parameters.size
        )
    );
}

}

namespace Lists {

class DisplayList {
private:
    std::vector<const std::function<void()>> commands;

    GLenum mode = GL_NONE;

public:
    template<typename Func>
    void addCommand(Func&& command) {
        commands.emplace_back(std::forward<Func>(command));
    }

    void setMode(GLenum newMode) {
        mode = newMode;
    }

    void execute() const {
        for (const auto& command : commands) {
            command();
        }
    }

    GLenum getMode() const {
        return mode;
    }
};

class DisplayListManager {
private:
    GLuint nextListIndex = 1;
    bool executingDisplayList;

    bool ignoreNextCallFlag;

    FAST_MAP_BI(GLuint, const DisplayList) displayLists;

    GLuint activeDisplayListIndex;
    DisplayList activeDisplayList;

public:
    // i dont know if this is even correct
    GLuint genDisplayLists(GLsizei range) {
        if (range <= 0) return 0;

        GLuint startIndex = nextListIndex;
        nextListIndex += range;
        for (GLsizei i = 0; i < range; ++i) {
            displayLists.insert({ startIndex + i, DisplayList() });
        }

        return startIndex;
    }

    void startDisplayList(GLuint list, GLenum mode) {
        if (isRecording() || isExecuting()) return;
        if (!isList(list)) return;

        activeDisplayListIndex = list;
        activeDisplayList = DisplayList();
        activeDisplayList.setMode(mode);
    }

    void ignoreNextCall() {
        if (!isRecording() || isExecuting()) return;
        ignoreNextCallFlag = true;
    }

    template<auto F, typename... Args>
    requires std::invocable<decltype(F), Args...>
    void addCommand(Args&&... args) {
        if (!isRecording() || isExecuting()) return;
        if (ignoreNextCallFlag) {
            ignoreNextCallFlag = false;
            return;
        }
        
        activeDisplayList.addCommand(
            [...args = std::forward<Args>(args)]() {
                F(args...);
            }
        );
    }

    void endDisplayList() {
        if (!isRecording() || isExecuting()) return;
        if (activeDisplayList.getMode() == GL_COMPILE_AND_EXECUTE) activeDisplayList.execute();

        displayLists.insert({ activeDisplayListIndex, activeDisplayList });

        activeDisplayListIndex = 0;
        activeDisplayList = DisplayList();
    }

    void deleteDisplayLists(GLuint list, GLsizei range) {
        if (range <= 0) return;

        for (GLsizei i = 0; i < range; ++i) {
            if (!isList(list)) continue;
            displayLists.erase(list + i);
        }
    }

    void callDisplayList(GLuint list) {
        if (isRecording() || isExecuting()) return;
        if (!isList(list)) return;

        executingDisplayList = true;
        displayLists[list].execute();
        executingDisplayList = false;
    }

    template<typename T>
    void callDisplayLists(GLsizei n, const T* lists) {
        executingDisplayList = true;
        for (GLsizei i = 0; i < n; ++i) {
            if (!isList(i)) continue;
            displayLists[static_cast<GLuint>(lists[i])].execute();
        }
        executingDisplayList = false;
    }

    bool isList(GLuint list) {
        return displayLists.find(list) != displayLists.end();
    }

    bool isRecording() {
        return activeDisplayListIndex != 0;
    }

    bool isExecuting() {
        return executingDisplayList;
    }
};

inline std::shared_ptr<DisplayListManager> displayListManager = std::make_shared<DisplayListManager>();

}


