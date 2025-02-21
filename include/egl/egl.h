#pragma once

#include "utils/defines.h"
#include "utils/types.h"

PUBLIC_API FunctionPtr eglGetProcAddress(const char*);

static inline void eglInit();