#include "wrappers/base.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

DEFINE(void, glClearDepth, (GLdouble d)) {
    glClearDepthf(static_cast<GLfloat>(d));
}