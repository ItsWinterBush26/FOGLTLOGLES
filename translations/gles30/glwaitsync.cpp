#include "base/base.h"

#include <GLES3/gl3.h>

OVERRIDE(void, glWaitSync, GLsync sync, GLbitfield flags, GLuint64 timeout) {
	original_glWaitSync(sync, flags, timeout);
}
