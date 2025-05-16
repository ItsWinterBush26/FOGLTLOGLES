#pragma once

#include "es/state_tracking.h"
#include "gles/ffp/enums.h"
#include "gles20/shader_overrides.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "utils/fast_map.h"

#include <cstddef>
#include <GLES/gl.h>
#include <GLES3/gl32.h>
#include <memory>
#include <stack>
#include <vector>

inline GLenum currentMatrixMode = GL_MODELVIEW;
inline glm::mat4 currentMatrix;
inline std::stack<glm::mat4> matrixStack;

namespace Immediate {

inline const std::string immediateModeVS = R"(#version 320 es

layout(location = 0) in vec4 iVertexPosition;
layout(location = 1) in vec3 iVertexNormal;
layout(location = 2) in vec4 iVertexColor;
layout(location = 3) in vec4 iVertexTexCoord;

uniform mat4 modelViewProjection;

out vec4 oVertexColor;
out vec4 oVertexTexCoord;

void main() {
    gl_Position = modelViewProjection * iVertexPosition;
    oVertexColor = iVertexColor;
    oVertexTexCoord = iVertexTexCoord;
})";

inline const std::string immediateModeFS = R"(#version 320 es
precision mediump float;

in vec4 iVertexColor;
in vec4 iVertexTextureCoord;

out vec4 oFragColor;

uniform bool uUseTexture;
uniform sampler2D uTexture;

void main() {
    if (uUseTexture) {
        oFragColor = texture(uTexture, iVertexTextureCoord.st) * iVertexColor;
    } else {
        oFragColor = iVertexColor;
    }
})";

struct VertexData {
    glm::vec4 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec4 texCoord;
};

class ImmediateModeState {
private:
    GLenum currentPrimitive;

    glm::vec3 currentNormal;
    glm::vec4 currentColor;
    glm::vec4 currentTexCoord;

    std::vector<VertexData> vertices;
    VertexData currentVertex;

    GLuint vao, vbo;
    GLuint drawerProgram;
    GLuint useTextureUniLoc, textureUniLoc;

    bool active;

public:
    ImmediateModeState() {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* vertexShaderSource = immediateModeVS.c_str();
        OV_glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        OV_glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* fragmentShaderSource = immediateModeFS.c_str();
        OV_glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        OV_glCompileShader(fragmentShader);

        drawerProgram = glCreateProgram();
        glAttachShader(drawerProgram, vertexShader);
        glAttachShader(drawerProgram, fragmentShader);

        OV_glLinkProgram(drawerProgram);

        useTextureUniLoc = glGetUniformLocation(drawerProgram, "uUseTeture");
        textureUniLoc = glGetUniformLocation(drawerProgram, "uTexture");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
            (void*) offsetof(VertexData, position)
        );
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
            (void*) offsetof(VertexData, normal)
        );
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
            (void*) offsetof(VertexData, color)
        );
        
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
            (void*) offsetof(VertexData, texCoord)
        );
    }

    ~ImmediateModeState() {
        glDeleteProgram(drawerProgram);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void reset() {
        vertices.clear();
        currentVertex = VertexData();
        currentNormal = glm::vec3();
        currentColor = glm::vec4();
        currentTexCoord = glm::vec4();
    }

    void begin(GLenum primitive) {
        if (active) return;

        this->reset();
        currentPrimitive = primitive;
        
        active = true;
    }

    void setNormal(const glm::vec3& normal) {
        if (!active) return;

        currentNormal = normal;
    }

    void setColor(const glm::vec4& color) {
        if (!active) return;

        currentColor = color;
    }

    void setTexCoord(const glm::vec4& texCoord) {
        if (!active) return;

        currentTexCoord = texCoord;
    }

    void advance(std::function<void(VertexData&)> applyVertexPosition) {
        if (!active) return;
        
        applyVertexPosition(currentVertex);
        currentVertex.normal = currentNormal;
        currentVertex.color = currentColor;
        currentVertex.texCoord = currentTexCoord;

        vertices.push_back(currentVertex);

        currentVertex = VertexData();
    }

    void end() {
        if (!active) return;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), 
            vertices.data(), GL_STREAM_DRAW
        );

        glUseProgram(drawerProgram);

        const bool isTextureEnabled = trackedStates->isCapabilityEnabled(GL_TEXTURE_2D);
        glUniform1i(useTextureUniLoc, isTextureEnabled ? GL_TRUE : GL_FALSE);
        if (isTextureEnabled) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, trackedStates->activeTextureState->boundTextures[GL_TEXTURE_2D]);

            glUniform1i(textureUniLoc, 0);
        }

        glBindVertexArray(vao);
        glDrawArrays(currentPrimitive, 0, vertices.size());
        
        this->reset();
        
        active = false;
    }

    VertexData& getCurrentVertex() {
        return this->currentVertex;
    }

    bool isActive() {
        return this->active;
    }
};


inline std::shared_ptr<ImmediateModeState> immediateModeState;
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

    template<auto F, typename... Args>
    void addCommand(Args&&... args) {
        static_assert(std::is_invocable_v<decltype(F), Args...>, "addCommand<...>(args...) must match the function signature");
        if (activeDisplayListIndex == 0) return;
        
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
