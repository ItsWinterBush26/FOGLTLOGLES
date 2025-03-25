#pragma once

#include "utils/defines.h"
#include "utils/types.h"

typedef FunctionPtr(PFNEGLGETPROCADDRESSPROC)(const char* procname);

inline PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress;

PUBLIC_API FunctionPtr eglGetProcAddress(const char*);

void eglInit();