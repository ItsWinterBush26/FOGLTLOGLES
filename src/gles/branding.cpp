#include "build_info.h"
#include "es/utils.h"
#include "gl/glext.h"
#include "gl/header.h"
#include "gles/main.h"
#include "main.h"
#include "utils/strings.h"

#include <GLES/gl.h>

#ifndef CAST_TO_CUBYTE
#define CAST_TO_CUBYTE(str) reinterpret_cast<const GLubyte*>(str)
#endif

const GLubyte* OV_glGetString(GLenum name);
void OV_glGetIntegerv(GLenum pname, int* v);
const GLubyte* OV_glGetStringi(GLenum pname, int index);
void OV_glEnable(GLenum cap);

inline std::string glVersion;
inline std::string rendererString;

inline std::string fakeExtensionsJoined;

void GLES::registerBrandingOverride() {
    glVersion = string_format(
        "4.0.0 (on ES %i.%i)",
        ESUtils::version.first,
        ESUtils::version.second
    );

    rendererString = string_format(
        "FOGLTLOGLES %s (on %s)", // TODO: implement versioning
        RENDERER_VERSION,
        glGetString(GL_RENDERER)
    );

    REGISTEROV(glGetString);
    REGISTEROV(glGetIntegerv);
    REGISTEROV(glGetStringi);
    REGISTEROV(glEnable)
}

const GLubyte* OV_glGetString(GLenum name) {
    switch (name) {
        case GL_VERSION:
            return CAST_TO_CUBYTE(glVersion.c_str());
        
        case GL_SHADING_LANGUAGE_VERSION:
            return CAST_TO_CUBYTE("4.60 FOGLTLOGLES"); // 1.50 for GL3.2

        case GL_VENDOR:
            return CAST_TO_CUBYTE("ThatMG393");

        case GL_RENDERER:
            return CAST_TO_CUBYTE(rendererString.c_str());

        case GL_EXTENSIONS:
            fakeExtensionsJoined = join_set(ESUtils::fakeExtensions, " ");
            return CAST_TO_CUBYTE(fakeExtensionsJoined.c_str());

        default:
            return glGetString(name);
    }
}

void OV_glGetIntegerv(GLenum pname, int* v) {
    switch (pname) {
        case GL_NUM_EXTENSIONS:
            (*v) = ESUtils::fakeExtensions.size();
            break;

        default:
            glGetIntegerv(pname, v);
            break;
    }
}

const GLubyte* OV_glGetStringi(GLenum pname, int index) {
    switch (pname) {
        case GL_EXTENSIONS:
            if (index < 0 || static_cast<size_t>(index) >= ESUtils::fakeExtensions.size()) return nullptr;
            {
                auto it = ESUtils::fakeExtensions.begin();
                std::advance(it, index);
                return CAST_TO_CUBYTE((*it).c_str());
            }

        default:
            return glGetStringi(pname, index);
    }
}

void OV_glEnable(GLenum cap) {
    switch (cap) {
        case GL_DEBUG_OUTPUT:
            // break;

        default:
            glEnable(cap);
            break;
    }
}
