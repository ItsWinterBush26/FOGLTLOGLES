#pragma once

#include "es/limits.h"
#include "es/utils.h"
#include "gl/header.h"
#include "gl/glext.h"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
            pname = GL_TEXTURE_MIN_LOD;
            return;

        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            if (param > ESLimits::MAX_TEXTURE_MAX_ANISOTROPY_EXT) param = ESLimits::MAX_TEXTURE_MAX_ANISOTROPY_EXT;
            return;
    }
}

inline void selectProperTexParami(GLenum target, GLenum& pname, GLint& param) {
    switch (pname) {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
            param = GL_CLAMP_TO_EDGE;
            return;
        case GL_TEXTURE_WRAP_R:
            if (ESUtils::version.first < 3) {
                LOGI("GL_TEXTURE_WRAP_R not supported on GLES 3.x below!");
                return;
            }
            
            param = GL_CLAMP_TO_EDGE;
            return;
    }
}
