#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glEnable, GLenum cap) {
	original_glEnable(cap);
}
