#include "base/base.h"

#include <GLES3/gl3.h>

OVERRIDE(void, glDeleteSync, GLsync sync) {
	original_glDeleteSync(sync);
}
