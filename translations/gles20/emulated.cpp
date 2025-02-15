#include "base/base.h"
#include "utils/convertion.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// Textures
OVERRIDEV(glTexImage2D, GLenum target, GLint l, GLint iform, GLsizei w, GLsizei h, GLint b, GLenum form, GLenum type, const void *p) {
    original_glTexImage2D(target, l, get_es_internalformat(iform), w, h, b, form, type, p);
}
