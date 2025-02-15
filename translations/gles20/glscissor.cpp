#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glScissor, GLint x, GLint y, GLsizei w, GLsizei h) {
	original_glScissor(x, y, w, h);
}
