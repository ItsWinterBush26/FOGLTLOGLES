#pragma once

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include "gl/header.h"
#include "gl/glext.h"

inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
           switch (target) {
               case GL_TEXTURE_2D:
               case GL_TEXTURE_CUBE_MAP:
               case GL_TEXTURE_3D:
               case GL_TEXTURE_2D_ARRAY:
                   pname = GL_TEXTURE_MIN_LOD;
                   return;
           }
           return;
           
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            if (param > 16.0f) param = 16.0f;
            return;
            
        case GL_TEXTURE_MIN_FILTER:
            if (target == GL_TEXTURE_EXTERNAL_OES) {
                if (param == GL_NEAREST_MIPMAP_NEAREST || 
                    param == GL_NEAREST_MIPMAP_LINEAR) {
                    param = GL_NEAREST;
                } else if (param == GL_LINEAR_MIPMAP_NEAREST || 
                           param == GL_LINEAR_MIPMAP_LINEAR) {
                    param = GL_LINEAR;
                }
            }
            return;
    }
}

inline void selectProperTexParami(GLenum target, GLenum& pname, GLint& param) {
    switch (pname) {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
            if (target == GL_TEXTURE_EXTERNAL_OES && param != GL_CLAMP_TO_EDGE) {
                param = GL_CLAMP_TO_EDGE;
            }
            return;
    }
}
