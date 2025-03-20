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
inline std::string rendererString;
inline std::string extensions;

void GLES::registerBrandingOverride() {
    glVersion = string_format(
        "3.2.0 (on ES %i.%i)",
        ESUtils::version.first,
        ESUtils::version.second
    );

    rendererString = string_format(
        "FOGLTLOGLES (on %s)",
        glGetString(GL_RENDERER)
    );

    std::unordered_set<str> temp = ESUtils::extensions;

    LOGI("Spoofing ARB_buffer_storage");
    temp.insert("ARB_buffer_storage");

    extensions = join_set(temp, " ");

    REGISTEROV(glGetString);
}

const GLubyte* OV_glGetString(GLenum name) {
    switch (name) {
        case GL_VERSION:
            return CAST_TO_CUBYTE(glVersion.c_str());
        
        case GL_SHADING_LANGUAGE_VERSION:
            return CAST_TO_CUBYTE("1.50 FOGLTLOGLES"); // 1.50 for GL3.2

        case GL_VENDOR:
            return CAST_TO_CUBYTE("ThatMG393");

        case GL_RENDERER:
            return CAST_TO_CUBYTE(rendererString.c_str());

        case GL_EXTENSIONS:
            return CAST_TO_CUBYTE(extensions.c_str());

        default:
            return glGetString(name);
    }
}
