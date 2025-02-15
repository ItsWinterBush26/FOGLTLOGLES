#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glClearColor, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	original_glClearColor(r, g, b, a);
}
