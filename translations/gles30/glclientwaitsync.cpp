#include "base/base.h"

#include <GLES3/gl3.h>

OVERRIDE(GLenum, glClientWaitSync, GLsync s, GLbitfield bf, GLuint64 t) {
	return original_glClientWaitSync(s, bf, t);
}
