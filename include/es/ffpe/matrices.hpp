#pragma once

#include "gles/ffp/enums.hpp"
#include "glm/ext/matrix_float4x4.hpp"

#include <GLES3/gl32.h>
#include <stack>
#include <unordered_map>

namespace FFPE::Rendering::Matrices {

struct Matrix {
    GLenum type;
    glm::mat4 matrix = glm::mat4(1.0f);

    std::stack<glm::mat4> stack;
};

namespace States {
    inline GLenum currentMatrixType = GL_MODELVIEW;

    inline std::unordered_map<GLenum, Matrix> matrices;
    inline Matrix* currentMatrix;

    inline glm::mat4 modelViewProjection;
}

inline const Matrix getMatrix(GLenum mode) {
    return States::matrices[mode];
}

inline const Matrix getCurrentMatrix() {
    return *States::currentMatrix;
}

inline const glm::mat4 getModelViewProjection() {
    return States::modelViewProjection;
}

inline const glm::mat4 recalculateModelViewProjection() {
    return States::modelViewProjection = getMatrix(GL_PROJECTION).matrix * getMatrix(GL_MODELVIEW).matrix;
}

inline void setCurrentMatrix(GLenum mode) {
    States::currentMatrixType = mode;
    States::currentMatrix = &States::matrices[mode];
}

inline void modifyCurrentMatrix(const std::function<glm::mat4(glm::mat4)>& newMatrix) {
    States::currentMatrix->matrix = newMatrix(States::currentMatrix->matrix);

    if (States::currentMatrixType == GL_MODELVIEW
     || States::currentMatrixType == GL_PROJECTION) recalculateModelViewProjection();
}

inline void pushCurrentMatrix() {
    States::currentMatrix->stack.push(States::currentMatrix->matrix);
}

inline void popTopMatrix() {
    if (States::currentMatrix->stack.empty()) return;
        
    States::currentMatrix->matrix = States::currentMatrix->stack.top();
    States::currentMatrix->stack.pop();

    if (States::currentMatrixType == GL_MODELVIEW
     || States::currentMatrixType == GL_PROJECTION) recalculateModelViewProjection();
}

}

