#pragma once

#include "gles/ffp/enums.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "utils/conversions.h"
#include "utils/fast_map.h"

#include <bit>
#include <cstddef>
#include <GLES3/gl32.h>
#include <memory>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// TODO: break out and move to specific files

namespace FFPE::States {
namespace VertexData {
    struct VertexRepresentation {
        glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 texCoord = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
            bool buffered;
            GLint size;
            GLenum type;
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

inline GLbitfield buildCurrentStatesBitfield() {
    return (
        AlphaTest::op | float_to_bits(AlphaTest::threshold) |
        ShadeModel::type // |
    );
}

}

namespace Matrices {
struct MatrixState {
    GLenum type;
    glm::mat4 matrix;

    std::stack<glm::mat4> stack;
}; 

class MatricesStateManager {
private:
    GLenum currentMatrixType = GL_MODELVIEW;

    std::unordered_map<GLenum, MatrixState> matrices;
    MatrixState* currentMatrix;

    glm::mat4 modelViewProjection;

public:
    MatricesStateManager() {
        matrices.insert({
            GL_MODELVIEW,
            { GL_MODELVIEW, glm::mat4(1.0f), std::stack<glm::mat4>() }
        });

        matrices.insert({
            GL_PROJECTION,
            { GL_PROJECTION, glm::mat4(1.0f), std::stack<glm::mat4>() }
        });

        matrices.insert({
            GL_TEXTURE,
            { GL_TEXTURE, glm::mat4(1.0f), std::stack<glm::mat4>() }
        });
    }

    void setCurrentMatrix(GLenum mode) {
        currentMatrixType = mode;
        currentMatrix = &matrices[mode];
    }

    void modifyCurrentMatrix(const std::function<glm::mat4(glm::mat4)>& newMatrix) {
        currentMatrix->matrix = newMatrix(currentMatrix->matrix);

        if (currentMatrixType == GL_MODELVIEW || currentMatrixType == GL_PROJECTION) {
            modelViewProjection = getMatrix(GL_MODELVIEW).matrix * getMatrix(GL_PROJECTION).matrix;
        }
    }

    void pushCurrentMatrix() {
        currentMatrix->stack.push(currentMatrix->matrix);
    }

    void popTopMatrix() {
        if (currentMatrix->stack.empty()) return;
        
        currentMatrix->matrix = currentMatrix->stack.top();
        currentMatrix->stack.pop();
    }

    const MatrixState getMatrix(GLenum mode) {
        return this->matrices[mode];
    }

    const MatrixState getCurrentMatrix() {
        return *this->currentMatrix;
    }

    const glm::mat4 getModelViewProjection() {
        return modelViewProjection;
    }
};

inline std::shared_ptr<MatricesStateManager> matricesStateManager;
}

namespace Lists {

class DisplayList {
private:
    std::vector<const std::function<void()>> commands;

    GLenum mode = GL_NONE;

public:
    void addCommand(const std::function<void()>& command) {
        commands.push_back(command);
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
    bool isExecuting;
    bool isCallBatched;

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
        if (activeDisplayListIndex != 0) return;
        if (displayLists.find(list) != displayLists.end()) {
            return;
        }

        activeDisplayListIndex = list;
        activeDisplayList = DisplayList();
        activeDisplayList.setMode(mode);
    }

    void ignoreNextCall() {
        if (activeDisplayListIndex == 0) return;
        ignoreNextCallFlag = true;
    }

    template<auto F, typename... Args>
    void addCommand(Args&&... args) {
        static_assert(std::is_invocable_v<decltype(F), Args...>, "addCommand<...>(args...) must match the function signature");
        if (activeDisplayListIndex == 0) return;
        if (ignoreNextCallFlag) {
            ignoreNextCallFlag = false;
            return;
        }
        
        activeDisplayList.addCommand(
            [a = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                std::apply(F, a);
            }
        );
    }

    void endDisplayList() {
        if (activeDisplayListIndex == 0) return;

        displayLists.insert({ activeDisplayListIndex, activeDisplayList });

        if (activeDisplayList.getMode() == GL_COMPILE_AND_EXECUTE) {
            callDisplayList(activeDisplayList);
        }

        activeDisplayListIndex = 0;
        activeDisplayList = DisplayList();
    }

    void deleteDisplayLists(GLuint list, GLsizei range) {
        if (displayLists.find(list) == displayLists.end()) return;
        if (range <= 0) return;

        for (GLsizei i = 0; i < range; ++i) {
            displayLists.erase(list + i);
        }
    }

    void callBeginBatch() {
        isExecuting = true;
        isCallBatched = true;
    }

    void callDisplayList(GLuint list) {
        if (activeDisplayListIndex != 0) return;
        if (displayLists.find(list) == displayLists.end()) return;

        if (!isCallBatched) {
            isExecuting = true;
        }
        displayLists[list].execute();
        if (!isCallBatched) {
            isExecuting = false;
        }
    }

    void callDisplayList(DisplayList list) {
        if (!isCallBatched) {
            isExecuting = true;
        }
        list.execute();
        if (!isCallBatched) {
            isExecuting = false;
        }
    }

    void callEndBatch() {
        isExecuting = false;
    }

    bool isList(GLuint list) {
        return displayLists.find(list) != displayLists.end();
    }

    bool isRecording() {
        return activeDisplayListIndex != 0 && !isExecuting;
    }
};

inline std::shared_ptr<DisplayListManager> displayListManager = std::make_shared<DisplayListManager>();

}


