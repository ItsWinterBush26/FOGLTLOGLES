#pragma once

#include "utils/defines.h"
#include "utils/types.h"

#include <EGL/egl.h>

inline PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress;

PUBLIC_API FunctionPtr eglGetProcAddress(const char*);

void eglInit();