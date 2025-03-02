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
        case GL_TEXTURE_LOD_BIAS:
            // GL_TEXTURE_LOD_BIAS is not in GLES 3.2, replace with GL_TEXTURE_MIN_LOD.
            pname = GL_TEXTURE_MIN_LOD;
            return;

        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            // GLES 3.2 does not guarantee anisotropic filtering support, check extension.
            if (!ESUtils::isExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
                param = 1.0f;
                return;
            }
            if (param > maxAniso) param = maxAniso;
            return;
    }
}

inline void selectProperTexParami(GLenum target, GLenum& pname, GLint& param) {
    switch (pname) {
        case GL_TEXTURE_WRAP_R:
            // GLES 3.2 requires GL_CLAMP_TO_EDGE if GL_EXT_texture_cube_map_array is not supported.
            if (!ESUtils::isExtensionSupported("GL_EXT_texture_cube_map_array")) {
                param = GL_CLAMP_TO_EDGE;
            }
            return;
    }
}
