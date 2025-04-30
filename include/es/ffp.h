#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "utils/fast_map.h"
#include <GLES/gl.h>
#include <memory>

#include <stack>
#include <vector>

#ifndef GL_NONE
#define GL_NONE 0
#endif

inline GLenum currentPrimitive = GL_NONE;

inline std::vector<GLfloat> floatVertexBuffer;

inline GLenum currentMatrixMode = GL_MODELVIEW;
inline glm::mat4 currentMatrix;
inline std::stack<glm::mat4> matrixStack;

namespace Lists {

class DisplayList {
private:
    std::vector<const std::function<void()>> commands;

public:
    void addCommand(const std::function<void()>& command) {
        commands.push_back(command);
    }

    void execute() const {
        for (const auto& command : commands) {
            command();
        }
    }
};

class DisplayListManager {
private:
    GLuint activeDisplayListIndex;
    DisplayList activeDisplayList;

    bool isExecuting;

    FAST_MAP_BI(GLuint, const DisplayList) displayLists;

public:
    void startDisplayList(GLuint list) {
        if (activeDisplayListIndex != 0) return;
        if (displayLists.find(list) != displayLists.end()) {
            return;
        }

        activeDisplayListIndex = list;
        activeDisplayList = DisplayList();
    }

    template<auto F, typename... Args>
    void addCommand(Args&&... args) {
        if (activeDisplayListIndex == 0) return;
        static_assert(std::is_invocable_v<decltype(F), Args...>, "addCommand<...>(args...) must match the function signature");

        activeDisplayList.addCommand(
            [a = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                std::apply(F, a);
            }
        );
    }

    void endDisplayList() {
        if (activeDisplayListIndex == 0) return;

        displayLists.insert({ activeDisplayListIndex, activeDisplayList });
        activeDisplayListIndex = 0;
        activeDisplayList = DisplayList();
    }

    void deleteDisplayList(GLuint list) {
        if (displayLists.find(list) == displayLists.end()) return;
        displayLists.erase(list);
    }

    void callDisplayList(GLuint list) {
        if (displayLists.find(list) == displayLists.end()) return;

        isExecuting = true;
        displayLists[list].execute();
        isExecuting = false;
    }

    bool isRecording() {
        return activeDisplayListIndex != 0 && !isExecuting;
    }
};

inline std::shared_ptr<DisplayListManager> displayListManager = std::make_shared<DisplayListManager>();

}
