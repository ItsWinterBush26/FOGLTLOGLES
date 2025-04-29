#pragma once

#include "utils/defines.h"
#include "utils/types.h"

#include <dlfcn.h>
#include <mutex>

inline std::once_flag eglInitFlag;
inline std::once_flag rendererInitFlag;

inline void* eglLibHandle = dlopen("libEGL.so", RTLD_LAZY | RTLD_LOCAL);

typedef FunctionPtr (*PFNEGLGETPROCADDRESSPROC)(const char* procname);

inline PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress;

PUBLIC_API FunctionPtr eglGetProcAddress(const char*);

void eglInit();