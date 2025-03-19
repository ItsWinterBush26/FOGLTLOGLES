#include "es/utils.h"
#include "gles/main.h"
#include "main.h"
#include "utils/strings.h"

#include <GLES/gl.h>

#ifndef CAST_TO_CUBYTE
#define CAST_TO_CUBYTE(str) reinterpret_cast<const GLubyte*>(str)
#endif

const GLubyte* OV_glGetString(GLenum name);

inline std::string glVersion;

void GLES::registerBrandingOverride() {
    glVersion = string_format(
        "OpenGL 3.2 (running on OpenGL ES %i.%i)",
        ESUtils::version.first,
        ESUtils::version.second
    );

    REGISTEROV(glGetString);
}

const GLubyte* OV_glGetString(GLenum name) {
    switch (name) {
        case GL_VERSION:
            return CAST_TO_CUBYTE(glVersion.c_str());
        
        case GL_SHADING_LANGUAGE_VERSION:
            return CAST_TO_CUBYTE("3.3 FOGLTLOGLES");

        case GL_VENDOR:
            return CAST_TO_CUBYTE("ThatMG393");

        default:
            return glGetString(name);
    }
}
