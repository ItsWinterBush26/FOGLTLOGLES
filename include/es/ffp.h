#pragma once

#include "es/binding_saver.h"
#include "es/state_tracking.h"
#include "gles/draw_overrides.h"
#include "gles/ffp/enums.h"
#include "gles20/shader_overrides.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils/fast_map.h"
#include "utils/log.h"

#include <cstddef>
#include <GLES3/gl32.h>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// TODO: break out and move to specific files

// lmao
#define MAX_TEXTURE_UNITS 64

namespace FFPE::States {
namespace AlphaTest {
    inline GLenum op = GL_ALWAYS;
    inline GLclampf threshold = 0.0f;
};

namespace ShadeModel {
    inline GLenum type = GL_SMOOTH;
};
namespace ClientState {
    namespace Arrays {
        struct ArrayParameters {
            bool buffered;
            GLint size;
            GLenum type;
            GLsizei stride;
            const void* firstElement;
        };
        
        struct ArrayState {
            bool enabled;
            GLenum bufferType;

            ArrayParameters parameters;

            void bind(GLuint buffer, GLint location, GLsizei count) {
                SaveBoundedBuffer* sbb;
                if (!parameters.buffered) {
                    sbb = new SaveBoundedBuffer(bufferType);

                    glBindBuffer(bufferType, buffer);
                    glBufferData(
                        bufferType,
                        parameters.size * count,
                        parameters.firstElement,
                        GL_STATIC_DRAW
                    );
                }

                // we assume a vao is already bound
                glEnableVertexAttribArray(location);
                glVertexAttribPointer(
                    location, parameters.size,
                    parameters.type, GL_FALSE, parameters.stride,
                    (parameters.buffered) ? parameters.firstElement : nullptr
                );

                if (!parameters.buffered) delete sbb;
            }
        };
        
        inline std::unordered_map<GLenum, ArrayState> arrayStates;

        inline bool isArrayEnabled(GLenum array) {
            return arrayStates[array].enabled;
        }

        inline ArrayState* getArray(GLenum array) {
            return &arrayStates[array];
        }
    }

    inline GLenum currentTexCoordTextureUnit;
    inline std::unordered_set<GLenum> texCoordTextureUnits;

}

namespace Rendering {
    inline GLuint universalVertexBuffer; // GL_ARRAY_BUFFER
    // inline GLuint universalIndicesBuffer; // GL_ELEMENT_ARRAY_BUFFER

    inline void init() {
        glGenBuffers(1, &universalVertexBuffer);
        // glGenBuffers(1, &universalIndicesBuffer);
    }
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
};

inline std::shared_ptr<MatricesStateManager> matricesStateManager;
}

namespace Immediate {

inline const std::string immediateModeVS = R"(#version 320 es

layout(location = 0) in vec4 iVertexPosition;
layout(location = 1) in vec3 iVertexNormal;
layout(location = 2) in vec4 iVertexColor;
layout(location = 3) in vec2 iVertexTexCoord;

uniform mat4 uModelViewProjection;

out vec4 vertexColor;
out vec2 vertexTexCoord;

void main() {
    gl_Position = uModelViewProjection * iVertexPosition;
    vertexColor = iVertexColor;
    vertexTexCoord = iVertexTexCoord;
})";

inline const std::string immediateModeFS = R"(#version 320 es
precision mediump float;

in vec4 vertexColor;
in vec2 vertexTexCoord;

out vec4 fragColor;

uniform bool uUseTexture;
uniform sampler2D texture0; // mc seems to only use one unit (good for us!)

// alpha test
uniform bool uAlphaTestEnabled;
uniform int uAlphaTestOp;
uniform float uAlphaTestThreshold;

void main() {
    vec4 resultColor;
    if (uUseTexture) {
        resultColor = texture(texture0, vertexTexCoord) * vertexColor;
    } else {
        resultColor = vertexColor;
    }

    if (uAlphaTestEnabled && uAlphaTestOp != 0x0207) {
        if (uAlphaTestOp == 0x0200) discard;
        bool alphaTestPassed = false;
        float resultColorAlpha = resultColor.a;

        if (uAlphaTestOp == 0x0201) alphaTestPassed = (resultColorAlpha < uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0201) alphaTestPassed = (resultColorAlpha < uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0202) alphaTestPassed = (resultColorAlpha == uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0203) alphaTestPassed = (resultColorAlpha <= uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0204) alphaTestPassed = (resultColorAlpha > uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0205) alphaTestPassed = (resultColorAlpha != uAlphaTestThreshold);
        else if (uAlphaTestOp == 0x0206) alphaTestPassed = (resultColorAlpha >= uAlphaTestThreshold);

        if (!alphaTestPassed) discard;
    }

    fragColor = resultColor;
})";

struct VertexGenericData {
    glm::vec4 position = glm::vec4(0, 0, 0, 1);
    glm::vec3 normal;
    glm::vec4 color;
};

struct VertexTexCoords {
    glm::vec2 texCoords[MAX_TEXTURE_UNITS];
};

class ImmediateModeState {
private:
    GLenum currentPrimitive;

    glm::vec3 currentNormal;
    glm::vec4 currentColor;
    std::vector<glm::vec2> currentTexCoords;

    std::vector<VertexGenericData> vertices;
    std::vector<VertexTexCoords> verticesTexCoords;
    VertexGenericData currentVertex;

    GLuint vao, vbo;
    GLuint drawerProgram;

    // vert
    GLuint modelViewProjectionUniLoc;

    // frag
    GLuint useTextureUniLoc, textureUniLoc;

    // alpha test
    GLuint alphaTestEnabledUniLoc, alphaTestOpUniLoc, alphaTestThresholdUniLoc;

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

        // vert
        modelViewProjectionUniLoc = glGetUniformLocation(drawerProgram, "uModelViewProjection");

        // frag
        useTextureUniLoc = glGetUniformLocation(drawerProgram, "uUseTexture");
        textureUniLoc = glGetUniformLocation(drawerProgram, "uTexture");

        // alpha
        alphaTestEnabledUniLoc = glGetUniformLocation(drawerProgram, "uAlphaTestEnabled");
        alphaTestOpUniLoc = glGetUniformLocation(drawerProgram, "uAlphaTestOp");
        alphaTestThresholdUniLoc = glGetUniformLocation(drawerProgram, "uAlphaTestThreshold");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // TODO: use our emulated gl*Pointer instead for cleanliness
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGenericData), 
            (void*) offsetof(VertexGenericData, position)
        );
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGenericData), 
            (void*) offsetof(VertexGenericData, normal)
        );
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGenericData), 
            (void*) offsetof(VertexGenericData, color)
        );
        
        /* glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
            (void*) offsetof(VertexData, texCoords)
        ); */
    }

    ~ImmediateModeState() {
        glDeleteProgram(drawerProgram);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void reset() {
        vertices.clear();
        currentVertex = VertexGenericData();
        currentNormal = glm::vec3(0, 0, 0);
        currentColor = glm::vec4(0, 0, 0, 1);
    }

    void begin(GLenum primitive) {
        if (active) return;
        LOGI("glBegin()!");

        this->reset();
        currentPrimitive = primitive;
        
        active = true;
    }

    void setNormal(const glm::vec3& normal) {
        currentNormal = normal;
    }

    void setColor(const glm::vec4& color) {
        currentColor = color;
    }

    void setTexCoord(const glm::vec2& texCoord) {
        currentTexCoords.push_back(texCoord);
    }

    void advance(std::function<void(VertexGenericData&)> applyVertexPosition) {
        if (!active) return;
        
        applyVertexPosition(currentVertex);
        currentVertex.normal = currentNormal;
        currentVertex.color = currentColor;
        // currentVertex.texCoord = currentTexCoord;

        vertices.push_back(currentVertex);

        currentVertex = VertexGenericData();
    }

    void end() {
        if (!active) return;
        LOGI("glEnd()!");

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(VertexGenericData), 
            vertices.data(), GL_STATIC_DRAW
        );

        glUseProgram(drawerProgram);

        const bool isTextureEnabled = trackedStates->isCapabilityEnabled(GL_TEXTURE_2D);
        glUniform1i(useTextureUniLoc, isTextureEnabled ? GL_TRUE : GL_FALSE);

        glm::mat4 finalMVP = Matrices::matricesStateManager->getMatrix(GL_MODELVIEW).matrix * Matrices::matricesStateManager->getMatrix(GL_PROJECTION).matrix;
        glUniformMatrix4fv(modelViewProjectionUniLoc, 1, false, glm::value_ptr(finalMVP));

        glUniform1i(alphaTestEnabledUniLoc, trackedStates->isCapabilityEnabled(GL_ALPHA_TEST) ? GL_TRUE : GL_FALSE);
        glUniform1i(alphaTestOpUniLoc, FFPE::States::AlphaTest::op);
        glUniform1f(alphaTestThresholdUniLoc, FFPE::States::AlphaTest::threshold);

        glBindVertexArray(vao);
        // will get called twice when we on a display list (glend->gldraw,gldraw) (unsure!)
        OV_glDrawArrays(currentPrimitive, 0, vertices.size());
        
        this->reset();
        
        active = false;
    }

    VertexGenericData& getCurrentVertex() {
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


