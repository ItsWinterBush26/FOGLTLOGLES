#pragma once

#include <GLES/gl.h>
#include <vector>

#ifndef GL_NONE
#define GL_NONE 0
#endif

inline GLenum currentPrimitive = GL_NONE;

inline std::vector<GLint> intVertexBuffer;
inline std::vector<GLfloat> floatVertexBuffer;
