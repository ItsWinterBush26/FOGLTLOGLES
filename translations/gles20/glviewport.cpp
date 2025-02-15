#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glViewport, GLint x, GLint y, GLsizei w, GLsizei h) {
	original_glViewport(x, y, w, h);
}
