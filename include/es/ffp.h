#pragma once

#include "glm/ext/matrix_double4x4.hpp"
#include "utils/fast_map.h"
#include <GLES/gl.h>
#include <memory>
#include <set>
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
        for (auto& command : commands) {
            command();
        }
    }
};

class DisplayListManager {
private:
    GLuint activeDisplayListIndex;
    DisplayList activeDisplayList = DisplayList();

    FAST_MAP_BI(GLuint, const DisplayList) displayLists;

public:
    void startDisplayList(GLuint list) {
        if (displayLists.find(list) != displayLists.end()) {
            return;
        }

        activeDisplayListIndex = list;
    }

    void addCommand(const std::function<void()>& command) {
        if (activeDisplayListIndex == 0) return;
        activeDisplayList.addCommand(command);
    }

    void endDisplayList() {
        if (activeDisplayListIndex == 0) return;

        displayLists.insert({ activeDisplayListIndex, activeDisplayList });
        activeDisplayListIndex = 0;
        activeDisplayList = DisplayList();
    }

    void callDisplayList(GLuint list) {
        if (displayLists.find(list) == displayLists.end()) return;
        displayLists[list].execute();
    }

    bool isRecording() {
        return activeDisplayListIndex != 0;
    }
};

inline std::shared_ptr<DisplayListManager> displayListManager = std::make_shared<DisplayListManager>();

}
