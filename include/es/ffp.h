#pragma once

#include "glm/ext/matrix_double4x4.hpp"
#include <GLES/gl.h>
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
