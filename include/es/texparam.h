#pragma once

#include <GLES3/gl3.h>
#include <stdexcept>

#include "es/utils.h"
#include "gl/header.h"

static inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    if (ESUtils::version.first < 3) throw std::runtime_error("Need GLES 3.x to emulated TEX_LOD_BIAS");
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
           pname = GL_TEXTURE_MIN_LOD;
           return;
    }
}