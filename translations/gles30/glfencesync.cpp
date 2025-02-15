#include "base/base.h"

#include <GLES3/gl3.h>

OVERRIDE(GLsync, glFenceSync, GLenum cond, GLbitfield flags) {
	return original_glFenceSync(cond, flags);
}
