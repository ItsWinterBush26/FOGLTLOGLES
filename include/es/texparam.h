#pragma once

#include "es/utils.h"
#include "gl/header.h"
#include "gl/glext.h"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <mutex>

inline std::once_flag initVarsFlag;

inline GLfloat maxAniso = 16.0f;

inline void initVars() {
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    LOGI("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT is %f", maxAniso);
}

inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    std::call_once(initVarsFlag, initVars);

    switch (pname) {
        /* case GL_TEXTURE_LOD_BIAS:
            // pname = GL_TEXTURE_MIN_LOD;
            return; */

        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            if (param > maxAniso) param = maxAniso;
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
