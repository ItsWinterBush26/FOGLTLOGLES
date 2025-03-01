#pragma once

#include <GLES3/gl3.h>

#include "gl/header.h"

inline void selectProperTexParamf(GLenum target, GLenum& pname, GLfloat& param) {
    switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
           switch (target) {
               case GL_TEXTURE_2D:
               case GL_TEXTURE_CUBE_MAP:
               case GL_TEXTURE_3D:           // Added 3D textures
               case GL_TEXTURE_2D_ARRAY:     // Added 2D array textures
                   pname = GL_TEXTURE_MIN_LOD;
                   return;
           }
           return;
           
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:  // Handle anisotropic filtering
            if (param > 16.0f) param = 16.0f; // Clamp to reasonable maximum
            return;
            
        case GL_TEXTURE_MIN_FILTER:          // Handle mipmap parameters
            if (target == GL_TEXTURE_EXTERNAL_OES) {
                // External textures don't support mipmaps
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
        case GL_TEXTURE_WRAP_R:
            // Handle external textures which only support CLAMP_TO_EDGE
            if (target == GL_TEXTURE_EXTERNAL_OES && param != GL_CLAMP_TO_EDGE) {
                param = GL_CLAMP_TO_EDGE;
            }
            return;
    }
}
