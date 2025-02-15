#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glDisable, GLenum cap) {
	original_glDisable(cap);
}
