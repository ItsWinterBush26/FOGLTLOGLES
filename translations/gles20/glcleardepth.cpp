#include "base/base.h"

#include <GLES2/gl2.h>

DEFINE(void, glClearDepth, GLdouble d) {
	glClearDepthf(static_cast<GLfloat>(d));
}
