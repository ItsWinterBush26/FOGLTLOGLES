#include "base/base.h"

#include <GLES2/gl2.h>

OVERRIDE(void, glClear, GLbitfield bf) {
	original_glClear(bf);
}
