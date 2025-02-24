#pragma once

#include <GLES3/gl3.h>

#include "gl/header.h"

static inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
           if (target == GL_TEXTURE_2D || target == GL_TEXTURE_CUBE_MAP)
               pname = GL_TEXTURE_MIN_LOD;
           return;
    }
}