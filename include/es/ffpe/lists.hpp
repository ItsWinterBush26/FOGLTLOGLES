#pragma once

#include "gles/ffp/enums.hpp"
#include "utils/fast_map.hpp"

#include <GLES3/gl32.h>
#include <functional>
#include <string>
#include <vector>

namespace FFPE::List {

class DisplayList {
private:
    std::vector<const std::function<void()>> commands;

    GLenum mode = GL_NONE;

public:
    DisplayList() { }
    DisplayList(GLenum mode) : mode(mode) { }
    
    template<typename Func>
    void addCommand(Func&& command) {
        commands.emplace_back(std::forward<Func>(command));
    }
    
    void setMode(GLenum mode) {
        this->mode = mode;
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

namespace States {
    inline GLuint nextListIndex = 1;
    inline bool executingDisplayList;

    inline bool ignoreNextCallFlag;

    inline FastMapBI<GLuint, DisplayList> displayLists;

    inline GLuint activeDisplayListIndex;
    inline DisplayList activeDisplayList;
}

inline bool isList(GLuint list) {
    return States::displayLists.find(list) != States::displayLists.end();
}

inline bool isRecording() {
    return States::activeDisplayListIndex != 0;
}

inline bool isExecuting() {
    return States::executingDisplayList;
}

// i dont know if this is even correct
inline GLuint genDisplayLists(GLsizei range) {
    if (range <= 0) return 0;

    for (GLsizei i = 0; i < range; ++i) {
        States::displayLists.emplace(
            i + States::nextListIndex, DisplayList()
        );
    }

    return std::exchange(States::nextListIndex, States::nextListIndex += range);
}

inline void startDisplayList(GLuint list, GLenum mode) {
    if (isRecording() || isExecuting()) return;
    if (!isList(list)) return;

    States::activeDisplayListIndex = list;
    States::activeDisplayList = DisplayList(mode);

    std::string dbgMes = "Recording list : " + std::to_string(list);
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, dbgMes.c_str());
}

inline void ignoreNextCall() {
    if (!isRecording() || isExecuting()) return;
    States::ignoreNextCallFlag = true;
}

template<auto F, typename... Args>
requires std::invocable<decltype(F), Args...>
inline bool addCommand(Args&&... args) {
    if (!isRecording() || isExecuting()) return false;
    if (States::ignoreNextCallFlag) {
        States::ignoreNextCallFlag = false;
        return false;
    }
    
    States::activeDisplayList.addCommand(
        [...args = std::forward<Args>(args)]() {
            F(args...);
        }
    );

    return true;
}

inline void endDisplayList() {
    if (!isRecording() || isExecuting()) return;
    if (States::activeDisplayList.getMode() == GL_COMPILE_AND_EXECUTE) States::activeDisplayList.execute();

    States::displayLists.insert({
        States::activeDisplayListIndex,
        States::activeDisplayList
    });

    States::activeDisplayListIndex = 0;
    States::activeDisplayList = DisplayList();

    glPopDebugGroup();
}

inline void deleteDisplayLists(GLuint list, GLsizei range) {
    if (range <= 0) return;

    for (GLsizei i = 0; i < range; ++i) {
        if (!isList(list)) continue;
        States::displayLists.erase(list + i);
    }
}

inline void callDisplayList(GLuint list) {
    if (isRecording() || isExecuting()) return;
    if (!isList(list)) return;

    std::string dbgMes = "List replay : " + std::to_string(list);
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, dbgMes.c_str());

    States::executingDisplayList = true;
    States::displayLists[list].execute();
    States::executingDisplayList = false;

    glPopDebugGroup();
}

template<typename T>
inline void callDisplayLists(GLsizei n, const T* lists) {
    States::executingDisplayList = true;
    for (GLsizei i = 0; i < n; ++i) {
        if (!isList(i)) continue;

        std::string dbgMes = "List replay : " + std::to_string(lists[i]);
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, dbgMes.c_str());

        States::displayLists[static_cast<GLuint>(lists[i])].execute();

        glPopDebugGroup();
    }
    States::executingDisplayList = false;
}

}
